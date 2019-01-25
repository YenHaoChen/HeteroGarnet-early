/*
 * Copyright (c) 2020 Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * For use for simulation and test purposes only
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Srikant Bharadwaj
 */


#include "mem/ruby/network/garnet2.0/NetworkBridge.hh"

#include <cmath>

#include "debug/RubyNetwork.hh"
#include "params/GarnetIntLink.hh"

NetworkBridge::NetworkBridge(const Params *p)
    :CreditLink(p)
{
    enCdc = true;
    enSerDes = true;
    mType = p->vtype;

    cdcLatency = p->cdc_latency;
    serDesLatency = p->serdes_latency;

    nLink = p->link;
    if (mType == FROM_LINK_) {
        nLink->setLinkConsumer(this);
        setSourceQueue(nLink->getBuffer(), nLink);
    } else if (mType == TO_LINK_) {
        nLink->setSourceQueue(&linkBuffer, this);
        setLinkConsumer(nLink);
    } else {
        // CDC type must be set
        panic("CDC type must be set");
    }

    lenBuffer.resize(p->vcs_per_vnet * p->virt_nets);
    extraCredit.resize(p->vcs_per_vnet * p->virt_nets);
}

void
NetworkBridge::init(NetworkBridge *coBrid, bool cdc_en, bool serdes_en)
{
    coBridge = coBrid;
    enCdc = cdc_en;
    enSerDes = serdes_en;
}

NetworkBridge::~NetworkBridge()
{
}

void
NetworkBridge::calculateLatency()
{
    // Change any calculations for cdc latency here
    // cdcLatency = Cycles(1);
}

void
NetworkBridge::scheduleFlit(flit *t_flit, Cycles latency)
{
    Cycles totLatency = latency;

    if (enCdc) {
        // Add the CDC latency
        totLatency = latency + cdcLatency;
    }

    t_flit->set_time(link_consumer->getObject()->clockEdge(totLatency));
    linkBuffer.insert(t_flit);
    link_consumer->scheduleEvent(totLatency);
}

void
NetworkBridge::neutralize(int vc, int eCredit)
{
    extraCredit[vc].push(eCredit);
}

void
NetworkBridge::flitisizeAndSend(flit *t_flit)
{
    // Serialize-Deserialize only if it is enabled
    if (enSerDes) {
        // Calculate the target-width
        uint32_t target_width = bitWidth;
        uint32_t cur_width = nLink->bitWidth;
        if (mType == TO_LINK_) {
            target_width = nLink->bitWidth;
            cur_width = bitWidth;
        }

        DPRINTF(RubyNetwork, "Target width: %d Current: %d Native: %d\n",
            target_width, cur_width, bitWidth);
        assert(target_width != cur_width);

        int vc = t_flit->get_vc();

        if (target_width > cur_width) {
            // Deserialize
            // This deserializer combines flits from the
            // same message together
            // TODO: Allow heterogenous flits
            int num_flits = 0;
            if (t_flit->get_type() == CREDIT_) {
                num_flits = (int)ceil((float)target_width/(float)cur_width);
            } else {
                num_flits = (int)ceil((float)t_flit->msgSize/(float)cur_width);
            }
            assert(num_flits > 0);

            DPRINTF(RubyNetwork, "Deserialize :%d -----> %d num:%d vc:%d\n",
                cur_width, target_width, num_flits, vc);

            // lenBuffer acts as the buffer for deserialization

            lenBuffer[vc]++;
            flit *fl = t_flit->deserialize(lenBuffer[vc], num_flits,
                target_width);

            if (t_flit->get_type() != CREDIT_ && fl) {
                coBridge->neutralize(vc, num_flits);
            }

            // Schedule only if we are done deserializing
            if (fl) {
                lenBuffer[vc] = 0;
                scheduleFlit(fl, serDesLatency);
            }
            // Delete this flit, new flit is sent in any case
            delete t_flit;
        } else {
            // Serialize
            DPRINTF(RubyNetwork, "Serialize :%d -----> %d vc:%d\n",
                cur_width, target_width);

            int num_parts = 0;
            if (t_flit->get_type() == CREDIT_) {
                assert(extraCredit[vc].front());
                num_parts = extraCredit[vc].front();
                extraCredit[vc].pop();
            } else {
                num_parts = (int)ceil((float)t_flit->msgSize/
                            (float)target_width);
            }
            assert(num_parts > 0);

            // Schedule all the flits
            // num_flits could be zero for credits
            for (int i = 0; i < num_parts; i++) {
                // Ignore neutralized credits
                flit *fl = t_flit->serialize(i, num_parts, target_width);
                scheduleFlit(fl, serDesLatency);
            }
            // Delete this flit, new flit is sent in any case
            delete t_flit;
        }
        return;
    }

    // If only CDC is enabled schedule it
    scheduleFlit(t_flit, Cycles(0));
}
void
NetworkBridge::wakeup()
{
    // Update CDC latency
    calculateLatency();

    flit *t_flit;

    if (link_srcQueue->isReady(curTick())) {
        t_flit = link_srcQueue->getTopFlit();
        flitisizeAndSend(t_flit);
        return;
    }
    assert(!link_srcQueue->getSize());
}

NetworkBridge *
NetworkBridgeParams::create()
{
    return new NetworkBridge(this);
}
