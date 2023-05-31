#pragma once

#include "StringTable.h"

using namespace RE;
class OverrideVariant
{
public:
	OverrideVariant() : type(kType_None), index(-1) { };
	~OverrideVariant() { };

	bool operator<(const OverrideVariant & rhs) const	{ return key < rhs.key || (key == rhs.key && index < rhs.index); }
	bool operator==(const OverrideVariant & rhs) const	{ return key == rhs.key && index == rhs.index; }

	std::uint16_t key;
	enum
	{
		kKeyMax = 0xFFFF,
		kIndexMax = 0xFF
	};
	enum
	{
		kParam_ShaderEmissiveColor = 0,
		kParam_ShaderEmissiveMultiple,
		kParam_ShaderGlossiness,
		kParam_ShaderSpecularStrength,
		kParam_ShaderLightingEffect1,
		kParam_ShaderLightingEffect2,
		kParam_ShaderTextureSet,
		kParam_ShaderTintColor,
		kParam_ShaderAlpha,
		kParam_ShaderTexture,

		kParam_ControllersStart = 20,
		kParam_ControllerStartStop = kParam_ControllersStart,
		kParam_ControllerStartTime,
		kParam_ControllerStopTime,
		kParam_ControllerFrequency,
		kParam_ControllerPhase,
		kParam_ControllersEnd = kParam_ControllerPhase,

		kParam_NodeTransformStart = 30,
		kParam_NodeTransformScale = kParam_NodeTransformStart,
		kParam_NodeTransformPosition,
		kParam_NodeTransformRotation,
		kParam_NodeTransformScaleMode,
		kParam_NodeTransformEnd = kParam_NodeTransformRotation,

		kParam_NodeDestination = 40
	};
	enum
	{
		kType_None = 0,
		kType_Identifier = 1,
		kType_String = 2,
		kType_Int = 3,
		kType_Float = 4,
		kType_Bool = 5
	};

	std::uint8_t	type;
	std::int8_t	index;
	union
	{
		std::int32_t			i;
		std::uint32_t			u;
		float			f;
		bool			b;
		void			* p;
	} data;
	StringTableItem		str;

	void	SetNone(void)
	{
		type = kType_None;
		index = -1;
		data.u = 0;
		str = nullptr;
	}

	void	SetInt(std::uint16_t paramKey, std::int8_t controllerIndex, std::int32_t i)
	{
		key = paramKey;
		type = kType_Int;
		index = controllerIndex;
		data.i = i;
		str = nullptr;
	}

	void	SetFloat(std::uint16_t paramKey, std::int8_t controllerIndex, float f)
	{
		key = paramKey;
		type = kType_Float;
		index = controllerIndex;
		data.f = f;
		str = nullptr;
	}

	void	SetBool(std::uint16_t paramKey, std::int8_t controllerIndex, bool b)
	{
		key = paramKey;
		type = kType_Bool;
		index = controllerIndex;
		data.b = b;
		str = nullptr;
	}

	void	SetString(std::uint16_t paramKey, std::int8_t controllerIndex, SKEEFixedString string)
	{
		key = paramKey;
		type = kType_String;
		index = controllerIndex;
		data.p = nullptr;
		str = std::shared_ptr<SKEEFixedString>(new SKEEFixedString(string));
	}

	void	SetColor(std::uint16_t paramKey, std::int8_t controllerIndex, NiColor color)
	{
		key = paramKey;
		type = kType_Int;
		index = controllerIndex;
		data.u = (std::uint8_t)(color.red * 255) << 16 | (std::uint8_t)(color.green * 255) << 8 | (std::uint8_t)(color.blue * 255);
		str = nullptr;
	}

	void	SetColorA(std::uint16_t paramKey, std::int8_t controllerIndex, NiColorA color)
	{
		key = paramKey;
		type = kType_Int;
		index = controllerIndex;
		data.u = (std::uint8_t)(color.alpha * 255) << 24 | (std::uint8_t)(color.red * 255) << 16 | (std::uint8_t)(color.green * 255) << 8 | (std::uint8_t)(color.blue * 255);
		str = nullptr;
	}

	void SetIdentifier(std::uint16_t paramKey, std::int8_t controllerIndex, void * ptr)
	{
		key = paramKey;
		type = kType_Identifier;
		index = controllerIndex;
		data.p = ptr;
		str = nullptr;
	}

	static bool IsIndexValid(std::uint16_t key) {
		return (key >= OverrideVariant::kParam_ControllersStart && key <= OverrideVariant::kParam_ControllersEnd) || key == OverrideVariant::kParam_ShaderTexture || (key >= OverrideVariant::kParam_NodeTransformStart && key <= OverrideVariant::kParam_NodeTransformEnd);
	};
};
