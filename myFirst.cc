#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("MyFirst");
int main(int argc,char* argv[]){
	CommandLine cmd(__FILE__);
    	cmd.Parse(argc, argv);
    	
    	LogComponentEnable("UdpEchoClientApplication",LOG_LEVEL_INFO);
    	LogComponentEnable("UdpEchoServerApplication",LOG_LEVEL_INFO);
	
	//helps setting up a pointToPoint connection
	PointToPointHelper p2p;
	p2p.SetDeviceAttribute("DataRate",StringValue("5Mbps"));
	p2p.SetChannelAttribute("Delay",StringValue("2ms"));
	
	//creating nodes
	NodeContainer nodes;
	nodes.Create(2);
	
	
	//setting up the pointToPoint connection between the nodes
	NetDeviceContainer devices;
	devices = p2p.Install(nodes);
	
	
	//setup internet stack
	InternetStackHelper stack;
	stack.Install(nodes);
	
	//IPv4 setup
	Ipv4AddressHelper addr;
	addr.SetBase("10.1.1.0","255.255.255.0");
	
	//assigning the interface to all devices present? 
	Ipv4InterfaceContainer interface = addr.Assign(devices);
	
	
	//simulation starts here
	UdpEchoServerHelper echoServer(8080); 
	
	ApplicationContainer serverApp = echoServer.Install(nodes.Get(1));
	serverApp.Start(Seconds(1.0));
	serverApp.Stop(Seconds(15.0));
	
	UdpEchoClientHelper echoClient(interface.GetAddress(1),8080);
	echoClient.SetAttribute("MaxPackets",UintegerValue(1));
	echoClient.SetAttribute("Interval",TimeValue(Seconds(1.0)));
	echoClient.SetAttribute("PacketSize",UintegerValue(1024));
	
	
	ApplicationContainer clientApp = echoClient.Install(nodes.Get(0));
	clientApp.Start(Seconds(3.0));
	clientApp.Stop(Seconds(12.0));
	
	Simulator::Run();
	Simulator::Destroy();
	return 0;
}