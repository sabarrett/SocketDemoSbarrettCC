//#include "RoboCatPCH.h"
//#include "LagSimulator.h"
//
//void LagSimulator::ReadIncomingPacketsIntoQueue(TCPSocketPtr mSocket, int byteReceives, InputMemoryBitStream inputBitstream)
//{
//	/*char packetMem[1500];
//	int packetSize = sizeof(packetMem);
//	InputMemoryBitStream inputStream(packetMem, packetSize * 8);*/
//	//SocketAddress fromAddress;
//
//	while (receivedPackedCount < kMaxPacketsPerFrameCount)
//	{
//		int cnt = byteReceives;
//		if (cnt == 0)
//		{
//			break;
//		}
//		else if (cnt < 0)
//		{
//			//handle error
//		}
//		else
//		{
//			//now, should we process the packet?
//			if (RoboMath::GetRandomFloat() >= mDropPacketChance)
//			{
//				//we made it, queue packet for later processing
//				float simulatedReceivedTime =
//					Timing::sInstance.GetTimef() +
//					mSimulatedLatency +
//					(RoboMath::GetRandomFloat() - 0.5f) *
//					mDoubleSimulatedMaxJitter;
//				//keep list sorted by simulated Receive time
//				auto it = mPacketList.end();
//				while (it != mPacketList.begin())
//				{
//					--it;
//					if (it->GetReceivedTime() < simulatedReceivedTime)
//					{
//						//time comes after this element, so inc and break
//						++it;
//						break;
//					}
//				}
//				mPacketList.emplace(it, simulatedReceivedTime,
//					inputBitstream);
//			}
//		}
//	}
//}