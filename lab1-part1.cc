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
    uint32_t nClients = 1;
    uint32_t nPackets = 1;
    
    CommandLine cmd(__FILE__);
    cmd.AddValue("nClients", "Number of client nodes (max 5)", nClients);
    cmd.AddValue("nPackets", "Number of packets per client (max 5)", nPackets);
    cmd.Parse(argc, argv);
    
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

    NodeContainer nodes;
    nodes.Create(1 + nClients); 
    
    Ptr<Node> serverNode = nodes.Get(1);
    
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));
    
    InternetStackHelper stack;
    stack.Install(nodes);
    
    NodeContainer firstLink(nodes.Get(0), serverNode);
    NetDeviceContainer firstDevices = pointToPoint.Install(firstLink);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer firstInterfaces = address.Assign(firstDevices);
    
    Ipv4Address serverAddress = firstInterfaces.GetAddress(1);
    
    for (uint32_t i = 1; i < nClients; ++i) {
        NodeContainer link(serverNode, nodes.Get(i + 1));
        NetDeviceContainer devices = pointToPoint.Install(link);
        
        std::ostringstream subnet;
        subnet << "10.1." << (i + 1) << ".0";
        address.SetBase(subnet.str().c_str(), "255.255.255.0");
        address.Assign(devices);
    }

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    UdpEchoServerHelper echoServer(15);
    ApplicationContainer serverApps = echoServer.Install(serverNode);
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(20.0));

    Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();
    rand->SetAttribute("Min", DoubleValue(2.0));
    rand->SetAttribute("Max", DoubleValue(7.0));
    
    for (uint32_t i = 0; i <= nClients; ++i) {
        if (i == 1) continue; 

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
