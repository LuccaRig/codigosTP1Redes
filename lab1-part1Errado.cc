#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/ipv4-global-routing-helper.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int
main(int argc, char* argv[])
{
    uint32_t nClients = 1;
    uint32_t nPackets = 1;

    CommandLine cmd(__FILE__);
    
    cmd.AddValue("nClients", "Number of client applications", nClients);
    cmd.AddValue("nPackets", "Number of packets to send", nPackets);
    
    cmd.Parse(argc, argv);

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    NodeContainer csmaNodes;
    csmaNodes.Add(p2pNodes.Get(1));  
    csmaNodes.Create(nClients);     

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install(csmaNodes);

    InternetStackHelper stack;
    stack.Install(p2pNodes.Get(0)); 
    stack.Install(csmaNodes);        

    Ipv4AddressHelper address;
    
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign(csmaDevices);

    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(csmaNodes.Get(0)); 
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    ApplicationContainer clientApps;
    for (uint32_t i = 1; i <= nClients; i++) { 

        UdpEchoClientHelper echoClient(csmaInterfaces.GetAddress(0), 9);  
        echoClient.SetAttribute("MaxPackets", UintegerValue(nPackets));
        echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
        echoClient.SetAttribute("PacketSize", UintegerValue(1024));

        double randomStartTime = 2.0 + (rand() % 6); 
        

        ApplicationContainer clientApp = echoClient.Install(csmaNodes.Get(i));
        clientApp.Start(Seconds(randomStartTime));  
        clientApp.Stop(Seconds(20.0));
        clientApps.Add(clientApp);
    }

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    pointToPoint.EnablePcapAll("multiclient");
    csma.EnablePcap("multiclient", csmaDevices.Get(0), true);

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}