#include "../Server/ClientWorkMap.h"
#include "../Server/ClientWorkMap.cpp"
#include "gtest/gtest.h"

#ifdef _DEBUG
#define CONF "Debug"
#else
#define CONF "Release"
#endif
#if defined _X86_
#define ARCH "x86"
#elif defined _AMD64_
#define ARCH "x64"
#endif

#pragma comment(lib, ARCH"/" CONF"/TCPCS") 

static void OnMessageReceived(TCPServInterface& serv, ClientData* const client, MsgStreamReader streamReader) {}

TEST(ClientWorkmap, Add_Valid) {
	ClientData clint(Socket{}, &OnMessageReceived);
	ClientWorkMap map{};
	map.Add(&clint, 0u, 100u, 1u);
	ASSERT_FALSE(map.Empty());
}

TEST(ClientWorkmap, Remove_Valid) {
	ClientData clint(Socket{}, &OnMessageReceived);
	ClientWorkMap map{};
	map.Add(&clint, 0u, 100u, 1u);
	const bool res = map.Remove(&clint);
	ASSERT_TRUE(res);
	ASSERT_TRUE(map.Empty());
}

TEST(ClientWorkmap, Remove_Invalid) {
	ClientData clint(Socket{}, &OnMessageReceived);
	ClientWorkMap map{};
	const bool res = map.Remove(&clint);
	ASSERT_FALSE(res);
	ASSERT_TRUE(map.Empty());
}

TEST(ClientWorkmap, GetClientWork_Valid) {
	ClientData clint(Socket{}, &OnMessageReceived);
	ClientWorkMap map{};
	map.Add(&clint, 0u, 100u, 1u);
	WorkInfo clintWorkInfo;
	map.GetClientWork(&clint, clintWorkInfo);
	ASSERT_EQ(clintWorkInfo.cursor, 0u);
	ASSERT_EQ(clintWorkInfo.size, 100u);
	ASSERT_EQ(clintWorkInfo.curIndex, 1u);
}