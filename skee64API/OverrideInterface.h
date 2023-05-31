#pragma once

#include "IPluginInterface.h"

#include "StringTable.h"
#include "OverrideVariant.h"

#include <set>
#include <vector>
#include <unordered_map>
#include <functional>

typedef std::uint32_t OverrideHandle;
class OverrideSet : public std::set<OverrideVariant>
{
public:
	virtual void Visit(std::function<bool(OverrideVariant*)> functor);
};

template<typename T>
class OverrideRegistration : public std::unordered_map<T, OverrideSet>
{
public:
	virtual void Visit(std::function<bool(const T& key, OverrideSet*)> functor);
};

class AddonRegistration : public std::unordered_map<OverrideHandle, OverrideRegistration<StringTableItem>>
{
public:
};

class ArmorRegistration : public std::unordered_map<OverrideHandle, AddonRegistration>
{
public:
};

class WeaponRegistration : public std::unordered_map<OverrideHandle, OverrideRegistration<StringTableItem>>
{
public:
};

class SkinRegistration : public std::unordered_map<OverrideHandle, OverrideSet>
{
public:
};

template<typename T, std::uint32_t N>
class MultiRegistration
{
public:
	// Serialization

	bool Load(SKSE::SerializationInterface* intfc, std::uint32_t kVersion, const StringIdMap& stringTable)
	{
		bool error = false;

		std::uint8_t size = 0;
		if (!intfc->ReadRecordData(&size, sizeof(size)))
		{
			logger::error("{} - Error loading multi-registrations", __FUNCTION__);
			error = true;
			return error;
		}

		for (std::uint32_t i = 0; i < size; i++)
		{
			std::uint8_t index = 0;
			if (!intfc->ReadRecordData(&index, sizeof(index)))
			{
				logger::error("{} - Error loading multi-registration index ({}/{})", __FUNCTION__, i + 1, size);
				error = true;
				return error;
			}

			T regs;
			if (regs.Load(intfc, kVersion, stringTable))
			{
				logger::error("{} - Error loading multi-registrations ({}/{})", __FUNCTION__, i + 1, size);
				error = true;
				return error;
			}

			table[index] = regs;
		}

		return error;
	}

	void Save(SKSE::SerializationInterface* intfc, std::uint32_t kVersion)
	{
		std::uint8_t size = 0;
		for (std::uint8_t i = 0; i < N; i++)
		{
			if (!table[i].empty())
				size++;
		}

		intfc->WriteRecordData(&size, sizeof(size));

		for (std::uint8_t i = 0; i < N; i++)
		{
			if (!table[i].empty())
			{
				intfc->WriteRecordData(&i, sizeof(i));
				table[i].Save(intfc, kVersion);
			}
		}
	}

	T& operator[] (const int index)
	{
		if (index > N - 1)
			return table[0];

		return table[index];
	}

	bool empty()
	{
		std::uint8_t emptyCount = 0;
		for (std::uint8_t i = 0; i < N; i++)
		{
			if (table[i].empty())
				emptyCount++;
		}
		return emptyCount == N;
	}

	T table[N];
};

class ActorRegistrationMapHolder : public SafeDataHolder<std::unordered_map<OverrideHandle, MultiRegistration<ArmorRegistration, 2>>>
{
public:
	typedef std::unordered_map<OverrideHandle, MultiRegistration<ArmorRegistration, 2>>	RegMap;
	friend class OverrideInterface;
};

class NodeRegistrationMapHolder : public SafeDataHolder<std::unordered_map<OverrideHandle, MultiRegistration<OverrideRegistration<StringTableItem>, 2>>>
{
public:
	typedef std::unordered_map<OverrideHandle, MultiRegistration<OverrideRegistration<StringTableItem>, 2>>	RegMap;
	friend class OverrideInterface;
};

class WeaponRegistrationMapHolder : public SafeDataHolder<std::unordered_map<OverrideHandle, MultiRegistration<MultiRegistration<WeaponRegistration, 2>, 2>>>
{
public:
	typedef std::unordered_map<OverrideHandle, MultiRegistration<MultiRegistration<WeaponRegistration, 2>, 2>>	RegMap;
	friend class OverrideInterface;
};

class SkinRegistrationMapHolder : public SafeDataHolder<std::unordered_map<OverrideHandle, MultiRegistration<MultiRegistration<SkinRegistration, 2>, 2>>>
{
public:
	typedef std::unordered_map<OverrideHandle, MultiRegistration<MultiRegistration<SkinRegistration, 2>, 2>>	RegMap;
	friend class OverrideInterface;
};

class OverrideInterface
	: public IPluginInterface
	, public IAddonAttachmentInterface
{
public:
	virtual std::uint32_t GetVersion();

	virtual void Save(SKSE::SerializationInterface* intfc, std::uint32_t kVersion);
	virtual bool Load(SKSE::SerializationInterface* intfc, std::uint32_t kVersion) { return false; };
	virtual void Revert();

	virtual bool LoadOverrides(SKSE::SerializationInterface* intfc, std::uint32_t kVersion, const StringIdMap& stringTable);
	virtual bool LoadNodeOverrides(SKSE::SerializationInterface* intfc, std::uint32_t kVersion, const StringIdMap& stringTable);
	virtual bool LoadWeaponOverrides(SKSE::SerializationInterface* intfc, std::uint32_t kVersion, const StringIdMap& stringTable);

	// Specific overrides
	virtual void AddRawOverride(OverrideHandle formId, bool isFemale, OverrideHandle armorHandle, OverrideHandle addonHandle, BSFixedString nodeName, OverrideVariant& value);
	virtual void AddOverride(TESObjectREFR* refr, bool isFemale, TESObjectARMO* armor, TESObjectARMA* addon, BSFixedString nodeName, OverrideVariant& value);

	// Non-specific overrides
	virtual void AddRawNodeOverride(OverrideHandle handle, bool isFemale, BSFixedString nodeName, OverrideVariant& value);
	virtual void AddNodeOverride(TESObjectREFR* refr, bool isFemale, BSFixedString nodeName, OverrideVariant& value);

	// Set/Get a single property
	virtual void SetArmorAddonProperty(TESObjectREFR* refr, bool isFemale, TESObjectARMO* armor, TESObjectARMA* addon, BSFixedString nodeName, OverrideVariant* value, bool immediate);
	virtual void GetArmorAddonProperty(TESObjectREFR* refr, bool firstPerson, TESObjectARMO* armor, TESObjectARMA* addon, BSFixedString nodeName, OverrideVariant* value);

	// Applies a single node property
	virtual void SetNodeProperty(TESObjectREFR* refr, BSFixedString nodeName, OverrideVariant* value, bool immediate);
	virtual void GetNodeProperty(TESObjectREFR* refr, bool firstPerson, BSFixedString nodeName, OverrideVariant* value);

	// Determines whether the node could be found
	virtual bool HasArmorAddonNode(TESObjectREFR* refr, bool firstPerson, TESObjectARMO* armor, TESObjectARMA* addon, BSFixedString nodeName, bool debug);

	// Applies all node overrides to a particular node
	virtual void ApplyNodeOverrides(TESObjectREFR* refr, NiAVObject* object, bool immediate);

	// Applies all armor overrides to a particular armor
	virtual void ApplyOverrides(TESObjectREFR* refr, TESObjectARMO* armor, TESObjectARMA* addon, NiAVObject* object, bool immediate);

	virtual void RemoveAllOverrides();
	virtual void RemoveAllReferenceOverrides(TESObjectREFR* reference);

	virtual void RemoveAllArmorOverrides(TESObjectREFR* refr, bool isFemale, TESObjectARMO* armor);
	virtual void RemoveAllArmorAddonOverrides(TESObjectREFR* refr, bool isFemale, TESObjectARMO* armor, TESObjectARMA* addon);
	virtual void RemoveAllArmorAddonNodeOverrides(TESObjectREFR* refr, bool isFemale, TESObjectARMO* armor, TESObjectARMA* addon, BSFixedString nodeName);
	virtual void RemoveArmorAddonOverride(TESObjectREFR* refr, bool isFemale, TESObjectARMO* armor, TESObjectARMA* addon, BSFixedString nodeName, std::uint16_t key, std::uint8_t index);

	virtual void RemoveAllNodeOverrides();
	virtual void RemoveAllReferenceNodeOverrides(TESObjectREFR* reference);

	virtual void RemoveAllNodeNameOverrides(TESObjectREFR* refr, bool isFemale, BSFixedString nodeName);
	virtual void RemoveNodeOverride(TESObjectREFR* refr, bool isFemale, BSFixedString nodeName, std::uint16_t key, std::uint8_t index);

	virtual OverrideVariant* GetOverride(TESObjectREFR* refr, bool isFemale, TESObjectARMO* armor, TESObjectARMA* addon, BSFixedString nodeName, std::uint16_t key, std::uint8_t index);
	virtual OverrideVariant* GetNodeOverride(TESObjectREFR* refr, bool isFemale, BSFixedString nodeName, std::uint16_t key, std::uint8_t index);

	virtual void AddRawWeaponOverride(OverrideHandle handle, bool isFemale, bool firstPerson, OverrideHandle weaponHandle, BSFixedString nodeName, OverrideVariant& value);
	virtual void AddWeaponOverride(TESObjectREFR* refr, bool isFemale, bool firstPerson, TESObjectWEAP* weapon, BSFixedString nodeName, OverrideVariant& value);
	virtual OverrideVariant* GetWeaponOverride(TESObjectREFR* refr, bool isFemale, bool firstPerson, TESObjectWEAP* weapon, BSFixedString nodeName, std::uint16_t key, std::uint8_t index);
	virtual void ApplyWeaponOverrides(TESObjectREFR* refr, bool firstPerson, TESObjectWEAP* weapon, NiAVObject* object, bool immediate);

	virtual void RemoveAllWeaponBasedOverrides();
	virtual void RemoveAllReferenceWeaponOverrides(TESObjectREFR* reference);

	virtual void RemoveAllWeaponOverrides(TESObjectREFR* refr, bool isFemale, bool firstPerson, TESObjectWEAP* weapon);
	virtual void RemoveAllWeaponNodeOverrides(TESObjectREFR* refr, bool isFemale, bool firstPerson, TESObjectWEAP* weapon, BSFixedString nodeName);
	virtual void RemoveWeaponOverride(TESObjectREFR* refr, bool isFemale, bool firstPerson, TESObjectWEAP* weapon, BSFixedString nodeName, std::uint16_t key, std::uint8_t index);

	virtual bool HasWeaponNode(TESObjectREFR* refr, bool firstPerson, TESObjectWEAP* weapon, BSFixedString nodeName, bool debug);
	virtual void SetWeaponProperties(OverrideHandle handle, bool immediate);

	virtual void SetWeaponProperty(TESObjectREFR* refr, bool isFemale, bool firstPerson, TESObjectWEAP* weapon, BSFixedString nodeName, OverrideVariant* value, bool immediate);
	virtual void GetWeaponProperty(TESObjectREFR* refr, bool firstPerson, TESObjectWEAP* weapon, BSFixedString nodeName, OverrideVariant* value);

	// Skin API
	virtual bool LoadSkinOverrides(SKSE::SerializationInterface* intfc, std::uint32_t kVersion, const StringIdMap& stringTable);
	virtual void AddRawSkinOverride(OverrideHandle handle, bool isFemale, bool firstPerson, std::uint32_t slotMask, OverrideVariant& value);
	virtual void AddSkinOverride(TESObjectREFR* refr, bool isFemale, bool firstPerson, std::uint32_t slotMask, OverrideVariant& value);
	virtual OverrideVariant* GetSkinOverride(TESObjectREFR* refr, bool isFemale, bool firstPerson, std::uint32_t slotMask, std::uint16_t key, std::uint8_t index);
	virtual void ApplySkinOverrides(TESObjectREFR* refr, bool firstPerson, TESObjectARMO* armor, TESObjectARMA* addon, std::uint32_t slotMask, NiAVObject* object, bool immediate);
	virtual void RemoveAllSkinOverrides(TESObjectREFR* refr, bool isFemale, bool firstPerson, std::uint32_t slotMask);
	virtual void RemoveSkinOverride(TESObjectREFR* refr, bool isFemale, bool firstPerson, std::uint32_t slotMask, std::uint16_t key, std::uint8_t index);
	virtual void SetSkinProperties(OverrideHandle handle, bool immediate);
	virtual void RemoveAllSkinBasedOverrides();
	virtual void RemoveAllReferenceSkinOverrides(TESObjectREFR* reference);
	virtual void SetSkinProperty(TESObjectREFR* refr, bool isFemale, bool firstPerson, std::uint32_t slotMask, OverrideVariant* value, bool immediate);
	virtual void GetSkinProperty(TESObjectREFR* refr, bool firstPerson, std::uint32_t slotMask, OverrideVariant* value);


	virtual void VisitNodes(TESObjectREFR* refr, std::function<void(SKEEFixedString, OverrideVariant&)> functor);
	virtual void VisitSkin(TESObjectREFR* refr, bool isFemale, bool firstPerson, std::function<void(std::uint32_t, OverrideVariant&)> functor);
	virtual void VisitStrings(std::function<void(SKEEFixedString)> functor);

private:
	ActorRegistrationMapHolder armorData;
	NodeRegistrationMapHolder nodeData;
	WeaponRegistrationMapHolder weaponData;
	SkinRegistrationMapHolder skinData;

	// Inherited via IAddonAttachmentInterface
	virtual void OnAttach(TESObjectREFR* refr, TESObjectARMO* armor, TESObjectARMA* addon, NiAVObject* object, bool isFirstPerson, NiNode* skeleton, NiNode* root) override;
};