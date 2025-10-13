/*
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * lab1-part2.cc
 * 
 * Based on second.cc example (ns-3 tutorial).
 * Adds a second point-to-point link from the last CSMA node to a new server node.
 * The number of packets (nPackets) can be set via command-line argument (default = 1, max = 20).
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Lab1Part2");

int main(int argc, char *argv[])
{
    bool verbose = true;
    uint32_t nCsma = 3;
    uint32_t nPackets = 1;

    CommandLine cmd(__FILE__);
    cmd.AddValue("nCsma", "Number of \"extra\" CSMA nodes/devices", nCsma);
    cmd.AddValue("nPackets", "Number of packets sent by the UdpEchoClient (max 20)", nPackets);
    cmd.AddValue("verbose", "Tell echo applications to log if true", verbose);
    cmd.Parse(argc, argv);

    if (nPackets > 20)
    {
        NS_LOG_WARN("nPackets value too high (" << nPackets << "), setting to 20");
        nPackets = 20;
    }

    if (verbose)
    {
        LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
    }

    nCsma = nCsma == 0 ? 1 : nCsma;

    // First P2P link between n0 and n1
    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    // CSMA nodes (n1, n2, n3, ..., n(1 + nCsma))
    NodeContainer csmaNodes;
    csmaNodes.Add(p2pNodes.Get(1));  // n1
    csmaNodes.Create(nCsma);         // n2..n(1 + nCsma)

    // Second P2P link between last CSMA node and new server node
    NodeContainer p2p2Nodes;
    p2p2Nodes.Add(csmaNodes.Get(nCsma)); // last CSMA node
    p2p2Nodes.Create(1);                 // new node (server)

    // Point-to-point configuration
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);

    // Second P2P link (same params)
    NetDeviceContainer p2p2Devices;
    p2p2Devices = pointToPoint.Install(p2p2Nodes);

    // CSMA configuration
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install(csmaNodes);

    // Install Internet stack
    InternetStackHelper stack;
    stack.Install(p2pNodes.Get(0));    // n0
    stack.Install(csmaNodes);          // n1..n(1 + nCsma)
    stack.Install(p2p2Nodes.Get(1));   // new server node

    // Assign IP addresses
    Ipv4AddressHelper address;

    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces = address.Assign(p2pDevices);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces = address.Assign(csmaDevices);

    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer p2p2Interfaces = address.Assign(p2p2Devices);

    // Create Echo Server on the new node (n5)
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(p2p2Nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(25.0));  // extended for 20 packets

    // Create Echo Client on n0
    UdpEchoClientHelper echoClient(p2p2Interfaces.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(nPackets));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(p2pNodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(25.0));

    // Enable routing
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Enable PCAP tracing for all links
    pointToPoint.EnablePcapAll("lab1-part2-p2p");
    csma.EnablePcap("lab1-part2-csma", csmaDevices.Get(1), true);
    pointToPoint.EnablePcapAll("lab1-part2-p2p2");

    Simulator::Stop(Seconds(26.0));
    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
