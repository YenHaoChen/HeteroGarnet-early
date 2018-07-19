/*
 * Copyright (c) 2008 Princeton University
 * Copyright (c) 2016 Georgia Institute of Technology
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "mem/ruby/network/garnet2.0/GarnetLink.hh"

#include "debug/RubyNetwork.hh"
#include "mem/ruby/network/garnet2.0/CreditLink.hh"
#include "mem/ruby/network/garnet2.0/NetworkBridge.hh"
#include "mem/ruby/network/garnet2.0/NetworkLink.hh"

GarnetIntLink::GarnetIntLink(const Params *p)
    : BasicIntLink(p)
{
    // Uni-directional

    m_network_link = p->network_link;
    m_credit_link = p->credit_link;

    srcCdcEn = p->src_cdc;
    dstCdcEn = p->dst_cdc;

    srcSerdesEn = p->src_serdes;
    dstSerdesEn = p->dst_serdes;

    srcBridgeEn = false;
    dstBridgeEn = false;

    if (srcCdcEn || srcSerdesEn) {
        srcBridgeEn = true;
    }
    if (dstCdcEn || dstSerdesEn) {
        dstBridgeEn = true;
    }

    srcNetBridge = p->src_net_bridge;
    dstNetBridge = p->dst_net_bridge;

    srcCredBridge = p->src_cred_bridge;
    dstCredBridge = p->dst_cred_bridge;

}

void
GarnetIntLink::init()
{
    srcNetBridge->init(srcCredBridge, srcCdcEn, srcSerdesEn);
    dstNetBridge->init(dstCredBridge, dstCdcEn, dstSerdesEn);
    srcCredBridge->init(srcNetBridge, srcCdcEn, srcSerdesEn);
    dstCredBridge->init(dstNetBridge, dstCdcEn, dstSerdesEn);
}

void
GarnetIntLink::print(std::ostream& out) const
{
    out << name();
}

GarnetIntLink *
GarnetIntLinkParams::create()
{
    return new GarnetIntLink(this);
}

GarnetExtLink::GarnetExtLink(const Params *p)
    : BasicExtLink(p)
{
    // Bi-directional

    // In
    m_network_links[0] = p->network_links[0];
    m_credit_links[0] = p->credit_links[0];

    // Out
    m_network_links[1] = p->network_links[1];
    m_credit_links[1] = p->credit_links[1];


    extCdcEn = p->ext_cdc;
    intCdcEn = p->int_cdc;

    extSerdesEn = p->ext_serdes;
    intSerdesEn = p->int_serdes;

    extBridgeEn = false;
    intBridgeEn = false;
    if (extCdcEn || extSerdesEn) {
        extBridgeEn = true;
    }
    if (intCdcEn || intSerdesEn) {
        intBridgeEn = true;
    }

    intNetBridge[0] = p->int_net_bridge[0];
    extNetBridge[0] = p->ext_net_bridge[0];

    intNetBridge[1] = p->int_net_bridge[1];
    extNetBridge[1] = p->ext_net_bridge[1];

    intCredBridge[0] = p->int_cred_bridge[0];
    extCredBridge[0] = p->ext_cred_bridge[0];

    intCredBridge[1] = p->int_cred_bridge[1];
    extCredBridge[1] = p->ext_cred_bridge[1];

}

void
GarnetExtLink::init()
{
    extNetBridge[0]->init(extCredBridge[0], extCdcEn, extSerdesEn);
    intNetBridge[0]->init(intCredBridge[0], intCdcEn, intSerdesEn);
    extNetBridge[1]->init(extCredBridge[1], extCdcEn, extSerdesEn);
    intNetBridge[1]->init(intCredBridge[1], intCdcEn, intSerdesEn);

    extCredBridge[0]->init(extNetBridge[0], extCdcEn, extSerdesEn);
    intCredBridge[0]->init(intNetBridge[0], intCdcEn, intSerdesEn);
    extCredBridge[1]->init(extNetBridge[1], extCdcEn, extSerdesEn);
    intCredBridge[1]->init(intNetBridge[1], intCdcEn, intSerdesEn);
}

void
GarnetExtLink::print(std::ostream& out) const
{
    out << name();
}

GarnetExtLink *
GarnetExtLinkParams::create()
{
    return new GarnetExtLink(this);
}
