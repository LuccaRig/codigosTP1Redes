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

    // Cria os nós
    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    // Cria nós CSMA para os clientes adicionais
    NodeContainer csmaNodes;
    csmaNodes.Add(p2pNodes.Get(1));  // Adiciona n1 como gateway
    csmaNodes.Create(nClients);      // Cria nós para os clientes

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

    NetDeviceContainer p2pDevices;
    p2pDevices = pointToPoint.Install(p2pNodes);

    // Cria rede CSMA para os clientes
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    NetDeviceContainer csmaDevices;
    csmaDevices = csma.Install(csmaNodes);

    InternetStackHelper stack;
    stack.Install(p2pNodes.Get(0));  // Instala no n0
    stack.Install(csmaNodes);        // Instala no n1 e nos clientes

    // Configura endereçamento IP
    Ipv4AddressHelper address;
    
    // Rede point-to-point (n0 - n1)
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces = address.Assign(p2pDevices);

    // Rede CSMA (clientes)
    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer csmaInterfaces;
    csmaInterfaces = address.Assign(csmaDevices);

    // Servidor no n1 (primeiro nó da rede CSMA)
    UdpEchoServerHelper echoServer(9);
    ApplicationContainer serverApps = echoServer.Install(csmaNodes.Get(0));  // n1
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(10.0));

    // Cria clientes em nós diferentes, cada um com seu próprio IP
    ApplicationContainer clientApps;
    for (uint32_t i = 1; i <= nClients; i++) {  // Começa de 1 para pular o n1 (servidor)
        // Cada cliente usa o IP do servidor (n1) como destino
        UdpEchoClientHelper echoClient(csmaInterfaces.GetAddress(0), 9);  // Servidor no índice 0
        echoClient.SetAttribute("MaxPackets", UintegerValue(nPackets));
        echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
        echoClient.SetAttribute("PacketSize", UintegerValue(1024));

        // Gera um tempo aleatório entre 2 e 7 segundos
        double randomStartTime = 2.0 + (rand() % 6); // 2.0 + (0 a 5) = 2 a 7 segundos
        
        // Instala cliente no nó CSMA correspondente
        ApplicationContainer clientApp = echoClient.Install(csmaNodes.Get(i));
        clientApp.Start(Seconds(randomStartTime));  // Tempo aleatório
        clientApp.Stop(Seconds(20.0));
        clientApps.Add(clientApp);
    }

    // Habilita roteamento global
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // Habilita captura de pacotes para debugging (opcional)
    pointToPoint.EnablePcapAll("multiclient");
    csma.EnablePcap("multiclient", csmaDevices.Get(0), true);

    Simulator::Run();
    Simulator::Destroy();
    return 0;
}