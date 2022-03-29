#pragma once
#ifndef ENUMS
enum MovementDir// : uint8_t
{
	MOVE_UP = 1 << 0, // binary 0001
	MOVE_DOWN = 1 << 1, // binary 0010
	MOVE_LEFT = 1 << 2, // binary 0100
	MOVE_RIGHT = 1 << 3  // binary 1000
};

enum PacketTypes// : uint8_t
{
	PKTTYPE_SETPLAYERID,
	PKTTYPE_WORLDSTATE,
	PKTTYPE_MOVEOBJECT,
	PKTTYPE_CREATEOBJECT,
	PKTTYPE_DESTROYOBJECT,
};

enum GameObjectTypes// : uint8_t
{
	GAMEOBJECT_BASE,
	GAMEOBJECT_COLORCHANGER,
	GAMEOBJECT_COLLECTABLE,
};
#endif // !ENUMS

