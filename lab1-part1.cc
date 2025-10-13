/*
 * SPDX-License-Identifier: GPL-2.0-only
 * 
 * Modified first.cc example to support multiple clients
 * with configurable number of clients and packets
 * 
 * Adjusted so that clients send requests to the same IP (10.1.1.2)
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include <cstdlib>
#include <ctime>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Lab1Part1");

int
main(int argc, char* argv[])
{
    srand(time(0));
    // Command line parameters with default values
    uint32_t nClients = 1;
    uint32_t nPackets = 1;
    
    CommandLine cmd(__FILE__);
    cmd.AddValue("nClients", "Number of client nodes (max 5)", nClients);
    cmd.AddValue("nPackets", "Number of packets per client (max 5)", nPackets);
    cmd.Parse(argc, argv);
    
    // Validate parameters
    if (nClients > 5) {
        nClients = 5;
        std::cout << "nClients limited to maximum of 5" << std::endl;
    }
    if (nPackets > 5) {
        nPackets = 5;
        std::cout << "nPackets limited to maximum of 5" << std::endl;
    }
    
    std::cout << "Running simulation with " << nClients << " clients, " 
              << nPackets << " packets per client" << std::endl;

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    // Create nodes: 1 server + nClients
    NodeContainer nodes;
    nodes.Create(1 + nClients);  // node 0 will be one client, node 1 is server, rest are other clients
    
    // Server is node 1 (address 10.1.1.2)
    Ptr<Node> serverNode = nodes.Get(1);
    
    // Create point-to-point helper
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
    
    // Install internet stack
    InternetStackHelper stack;
    stack.Install(nodes);
    
    // --- Create first link between node 0 (client) and node 1 (server) ---
    NodeContainer firstLink(nodes.Get(0), serverNode);
    NetDeviceContainer firstDevices = pointToPoint.Install(firstLink);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer firstInterfaces = address.Assign(firstDevices);
    
    // Server will use address 10.1.1.2 (the second device of the first link)
    Ipv4Address serverAddress = firstInterfaces.GetAddress(1);
    
    // --- Create remaining links (server <-> each other client) ---
    for (uint32_t i = 1; i < nClients; ++i) {
        NodeContainer link(serverNode, nodes.Get(i + 1));
        NetDeviceContainer devices = pointToPoint.Install(link);
        
        std::ostringstream subnet;
        subnet << "10.1." << (i + 1) << ".0";
        address.SetBase(subnet.str().c_str(), "255.255.255.0");
        address.Assign(devices);
    }

    // Populate routing tables
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // --- Install UDP Echo Server on node 1 ---
    UdpEchoServerHelper echoServer(15);
    ApplicationContainer serverApps = echoServer.Install(serverNode);
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(20.0));

    Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();
    rand->SetAttribute("Min", DoubleValue(2.0));
    rand->SetAttribute("Max", DoubleValue(7.0));
    
    // --- Install UDP Echo Clients on all other nodes (except node 1) ---
    for (uint32_t i = 0; i <= nClients; ++i) {
        if (i == 1) continue; // skip server node

        UdpEchoClientHelper echoClient(serverAddress, 15);
        echoClient.SetAttribute("MaxPackets", UintegerValue(nPackets));
        echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
        echoClient.SetAttribute("PacketSize", UintegerValue(1024));
        

        double randomStartTime = rand->GetValue();
        ApplicationContainer clientApp = echoClient.Install(nodes.Get(i));
        clientApp.Start(Seconds(randomStartTime));  
        clientApp.Stop(Seconds(20.0));
    }

    std::cout << "Server is at: " << serverAddress << std::endl;
    std::cout << "All clients sending to server at: " << serverAddress << std::endl;

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
