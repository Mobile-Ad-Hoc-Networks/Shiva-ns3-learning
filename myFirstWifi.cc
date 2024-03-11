#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"
/*
AP
*	*	*	*
|	|	|	|
n0 	n1	n2	n3

Wifi 10.1.1.0

*/
using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstWifiScript");
int main(int argc, char* argv[]){
	LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
        LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
        
        NodeContainer wifiAp;//one access point
        wifiAp.Create(1);
        
        NodeContainer wifiSta;//station devices -> clients?
        wifiSta.Create(3);
        
        YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
        YansWifiPhyHelper phy;
        phy.SetChannel(channel.Create());
        
        WifiMacHelper mac;
        Ssid ssid = Ssid("ns-3-ssid"); 
        
        WifiHelper wifi;
        
        NetDeviceContainer staDev;
        mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
        staDev = wifi.Install(phy,mac,wifiSta);
        
        NetDeviceContainer apDev;
        mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
        apDev = wifi.Install(phy,mac,wifiAp);
        
        MobilityHelper mob;
        mob.SetPositionAllocator("ns3::GridPositionAllocator",
        			"MinX",DoubleValue(0.0),
        			"MinY",DoubleValue(0.0),
        			"DeltaX",DoubleValue(10.0),
        			"DeltaY",DoubleValue(10.0),
        			"GridWidth",UintegerValue(3),
        			"LayoutType",StringValue("RowFirst"));//try out RandomDiscPositionAllocator and RandomRectanglePositionAllocator
        mob.SetMobilityModel("ns3::RandomWalk2dMobilityModel","Bounds",
        			RectangleValue(Rectangle(-50,50,-50,50)));
        mob.Install(wifiSta);
        
        mob.SetMobilityModel("ns3::ConstantPositionMobilityModel");
        mob.Install(wifiAp);
        
        InternetStackHelper stack;
        stack.Install(wifiSta);
        stack.Install(wifiAp);
        
        
        Ipv4AddressHelper addr;
	addr.SetBase("10.1.1.0", "255.255.255.0");
	Ipv4InterfaceContainer staInterfaces;
	staInterfaces = addr.Assign(staDev);
	Ipv4InterfaceContainer apInterfaces;
	apInterfaces = addr.Assign(apDev);

	// Install UDP Echo Server on the Access point
	UdpEchoServerHelper echoServer(9);
	ApplicationContainer serverApps = echoServer.Install(wifiAp);
	serverApps.Start(Seconds(1.0));
	serverApps.Stop(Seconds(10.0));

	// Install UDP Echo Client on one of the station nodes
	// Use the first station node's IP address for the echo client
	UdpEchoClientHelper echoClient(staInterfaces.GetAddress(0), 8080);
	echoClient.SetAttribute("MaxPackets", UintegerValue(10));
	echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
	echoClient.SetAttribute("PacketSize", UintegerValue(1024));
	ApplicationContainer clientApps = echoClient.Install(wifiSta.Get(0));
	clientApps.Start(Seconds(2.0));
	clientApps.Stop(Seconds(10.0));
	
	
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	Simulator::Stop(Seconds(10.0));
    	Simulator::Run();
    	Simulator::Destroy();

    	return 0;
        
        
}