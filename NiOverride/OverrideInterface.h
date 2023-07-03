#pragma once

#include "IPluginInterface.h"

#include "StringTable.h"
#include "OverrideVariant.h"

#include <set>
#include <vector>
#include <unordered_map>
#include <functional>

class OverrideSet : public std::set<OverrideVariant>
{
public:
	virtual void Visit(std::function<bool(OverrideVariant*)> functor) = 0;
};

template<typename T>
class OverrideRegistration : public std::unordered_map<T, OverrideSet>
{
public:
	virtual void Visit(std::function<bool(const T& key, OverrideSet*)> functor) = 0;
};

class AddonRegistration : public std::unordered_map<RE::VMHandle, OverrideRegistration<StringTableItem>>
{
public:
};

class ArmorRegistration : public std::unordered_map<RE::VMHandle, AddonRegistration>
{
public:
};

class WeaponRegistration : public std::unordered_map<RE::VMHandle, OverrideRegistration<StringTableItem>>
{
public:
};

class SkinRegistration : public std::unordered_map<RE::VMHandle, OverrideSet>
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

class ActorRegistrationMapHolder : public SafeDataHolder<std::unordered_map<RE::VMHandle, MultiRegistration<ArmorRegistration, 2>>>
{
public:
	typedef std::unordered_map<RE::VMHandle, MultiRegistration<ArmorRegistration, 2>> RegMap;
	friend class OverrideInterface;
};

class NodeRegistrationMapHolder : public SafeDataHolder<std::unordered_map<RE::VMHandle, MultiRegistration<OverrideRegistration<StringTableItem>, 2>>>
{
public:
	typedef std::unordered_map<RE::VMHandle, MultiRegistration<OverrideRegistration<StringTableItem>, 2>> RegMap;
	friend class OverrideInterface;
};

class WeaponRegistrationMapHolder : public SafeDataHolder<std::unordered_map<RE::VMHandle, MultiRegistration<MultiRegistration<WeaponRegistration, 2>, 2>>>
{
public:
	typedef std::unordered_map<RE::VMHandle, MultiRegistration<MultiRegistration<WeaponRegistration, 2>, 2>> RegMap;
	friend class OverrideInterface;
};

class SkinRegistrationMapHolder : public SafeDataHolder<std::unordered_map<RE::VMHandle, MultiRegistration<MultiRegistration<SkinRegistration, 2>, 2>>>
{
public:
	typedef std::unordered_map<RE::VMHandle, MultiRegistration<MultiRegistration<SkinRegistration, 2>, 2>> RegMap;
	friend class OverrideInterface;
};

class OverrideInterface
	: public IPluginInterface
	, public IAddonAttachmentInterface
{
public:
	virtual std::uint32_t GetVersion() = 0;

	virtual void Save(SKSE::SerializationInterface* intfc, std::uint32_t kVersion) = 0;
	virtual bool Load(SKSE::SerializationInterface* intfc, std::uint32_t kVersion) = 0;
	virtual void Revert() = 0;

	virtual bool LoadOverrides(SKSE::SerializationInterface* intfc, std::uint32_t kVersion, const StringIdMap& stringTable) = 0;
	virtual bool LoadNodeOverrides(SKSE::SerializationInterface* intfc, std::uint32_t kVersion, const StringIdMap& stringTable) = 0;
	virtual bool LoadWeaponOverrides(SKSE::SerializationInterface* intfc, std::uint32_t kVersion, const StringIdMap& stringTable) = 0;

	// Specific overrides
	virtual void AddRawOverride(RE::VMHandle formId, bool isFemale, RE::VMHandle armorHandle, RE::VMHandle addonHandle, RE::BSFixedString nodeName, OverrideVariant& value) = 0;
	virtual void AddOverride(RE::TESObjectREFR* refr, bool isFemale, RE::TESObjectARMO* armor, RE::TESObjectARMA* addon, RE::BSFixedString nodeName, OverrideVariant& value) = 0;

	// Non-specific overrides
	virtual void AddRawNodeOverride(RE::VMHandle handle, bool isFemale, RE::BSFixedString nodeName, OverrideVariant& value) = 0;
	virtual void AddNodeOverride(RE::TESObjectREFR* refr, bool isFemale, RE::BSFixedString nodeName, OverrideVariant& value) = 0;

	// Set/Get a single property
	virtual void SetArmorAddonProperty(RE::TESObjectREFR* refr, bool isFemale, RE::TESObjectARMO* armor, RE::TESObjectARMA* addon, RE::BSFixedString nodeName, OverrideVariant* value, bool immediate) = 0;
	virtual void GetArmorAddonProperty(RE::TESObjectREFR* refr, bool firstPerson, RE::TESObjectARMO* armor, RE::TESObjectARMA* addon, RE::BSFixedString nodeName, OverrideVariant* value) = 0;

	// Applies a single node property
	virtual void SetNodeProperty(RE::TESObjectREFR* refr, RE::BSFixedString nodeName, OverrideVariant* value, bool immediate) = 0;
	virtual void GetNodeProperty(RE::TESObjectREFR* refr, bool firstPerson, RE::BSFixedString nodeName, OverrideVariant* value) = 0;

	// Determines whether the node could be found
	virtual bool HasArmorAddonNode(RE::TESObjectREFR* refr, bool firstPerson, RE::TESObjectARMO* armor, RE::TESObjectARMA* addon, RE::BSFixedString nodeName, bool debug) = 0;

	// Applies all node overrides to a particular node
	virtual void ApplyNodeOverrides(RE::TESObjectREFR* refr, RE::NiAVObject* object, bool immediate) = 0;

	// Applies all armor overrides to a particular armor
	virtual void ApplyOverrides(RE::TESObjectREFR* refr, RE::TESObjectARMO* armor, RE::TESObjectARMA* addon, RE::NiAVObject* object, bool immediate) = 0;

	virtual void RemoveAllOverrides() = 0;
	virtual void RemoveAllReferenceOverrides(RE::TESObjectREFR* reference) = 0;

	virtual void RemoveAllArmorOverrides(RE::TESObjectREFR* refr, bool isFemale, RE::TESObjectARMO* armor) = 0;
	virtual void RemoveAllArmorAddonOverrides(RE::TESObjectREFR* refr, bool isFemale, RE::TESObjectARMO* armor, RE::TESObjectARMA* addon) = 0;
	virtual void RemoveAllArmorAddonNodeOverrides(RE::TESObjectREFR* refr, bool isFemale, RE::TESObjectARMO* armor, RE::TESObjectARMA* addon, RE::BSFixedString nodeName) = 0;
	virtual void RemoveArmorAddonOverride(RE::TESObjectREFR* refr, bool isFemale, RE::TESObjectARMO* armor, RE::TESObjectARMA* addon, RE::BSFixedString nodeName, std::uint16_t key, std::uint8_t index) = 0;

	virtual void RemoveAllNodeOverrides() = 0;
	virtual void RemoveAllReferenceNodeOverrides(RE::TESObjectREFR* reference) = 0;

	virtual void RemoveAllNodeNameOverrides(RE::TESObjectREFR* refr, bool isFemale, RE::BSFixedString nodeName) = 0;
	virtual void RemoveNodeOverride(RE::TESObjectREFR* refr, bool isFemale, RE::BSFixedString nodeName, std::uint16_t key, std::uint8_t index) = 0;

	virtual OverrideVariant* GetOverride(RE::TESObjectREFR* refr, bool isFemale, RE::TESObjectARMO* armor, RE::TESObjectARMA* addon, RE::BSFixedString nodeName, std::uint16_t key, std::uint8_t index) = 0;
	virtual OverrideVariant* GetNodeOverride(RE::TESObjectREFR* refr, bool isFemale, RE::BSFixedString nodeName, std::uint16_t key, std::uint8_t index) = 0;

	virtual void AddRawWeaponOverride(RE::VMHandle handle, bool isFemale, bool firstPerson, RE::VMHandle weaponHandle, RE::BSFixedString nodeName, OverrideVariant& value) = 0;
	virtual void AddWeaponOverride(RE::TESObjectREFR* refr, bool isFemale, bool firstPerson, RE::TESObjectWEAP* weapon, RE::BSFixedString nodeName, OverrideVariant& value) = 0;
	virtual OverrideVariant* GetWeaponOverride(RE::TESObjectREFR* refr, bool isFemale, bool firstPerson, RE::TESObjectWEAP* weapon, RE::BSFixedString nodeName, std::uint16_t key, std::uint8_t index) = 0;
	virtual void ApplyWeaponOverrides(RE::TESObjectREFR* refr, bool firstPerson, RE::TESObjectWEAP* weapon, RE::NiAVObject* object, bool immediate) = 0;

	virtual void RemoveAllWeaponBasedOverrides() = 0;
	virtual void RemoveAllReferenceWeaponOverrides(RE::TESObjectREFR* reference) = 0;

	virtual void RemoveAllWeaponOverrides(RE::TESObjectREFR* refr, bool isFemale, bool firstPerson, RE::TESObjectWEAP* weapon) = 0;
	virtual void RemoveAllWeaponNodeOverrides(RE::TESObjectREFR* refr, bool isFemale, bool firstPerson, RE::TESObjectWEAP* weapon, RE::BSFixedString nodeName) = 0;
	virtual void RemoveWeaponOverride(RE::TESObjectREFR* refr, bool isFemale, bool firstPerson, RE::TESObjectWEAP* weapon, RE::BSFixedString nodeName, std::uint16_t key, std::uint8_t index) = 0;

	virtual bool HasWeaponNode(RE::TESObjectREFR* refr, bool firstPerson, RE::TESObjectWEAP* weapon, RE::BSFixedString nodeName, bool debug) = 0;
	virtual void SetWeaponProperties(RE::VMHandle handle, bool immediate) = 0;

	virtual void SetWeaponProperty(RE::TESObjectREFR* refr, bool isFemale, bool firstPerson, RE::TESObjectWEAP* weapon, RE::BSFixedString nodeName, OverrideVariant* value, bool immediate) = 0;
	virtual void GetWeaponProperty(RE::TESObjectREFR* refr, bool firstPerson, RE::TESObjectWEAP* weapon, RE::BSFixedString nodeName, OverrideVariant* value) = 0;

	// Skin API
	virtual bool LoadSkinOverrides(SKSE::SerializationInterface* intfc, std::uint32_t kVersion, const StringIdMap& stringTable) = 0;
	virtual void AddRawSkinOverride(RE::VMHandle handle, bool isFemale, bool firstPerson, std::uint32_t slotMask, OverrideVariant& value) = 0;
	virtual void AddSkinOverride(RE::TESObjectREFR* refr, bool isFemale, bool firstPerson, std::uint32_t slotMask, OverrideVariant& value) = 0;
	virtual OverrideVariant* GetSkinOverride(RE::TESObjectREFR* refr, bool isFemale, bool firstPerson, std::uint32_t slotMask, std::uint16_t key, std::uint8_t index) = 0;
	virtual void ApplySkinOverrides(RE::TESObjectREFR* refr, bool firstPerson, RE::TESObjectARMO* armor, RE::TESObjectARMA* addon, std::uint32_t slotMask, RE::NiAVObject* object, bool immediate) = 0;
	virtual void RemoveAllSkinOverrides(RE::TESObjectREFR* refr, bool isFemale, bool firstPerson, std::uint32_t slotMask) = 0;
	virtual void RemoveSkinOverride(RE::TESObjectREFR* refr, bool isFemale, bool firstPerson, std::uint32_t slotMask, std::uint16_t key, std::uint8_t index) = 0;
	virtual void SetSkinProperties(RE::VMHandle handle, bool immediate) = 0;
	virtual void RemoveAllSkinBasedOverrides() = 0;
	virtual void RemoveAllReferenceSkinOverrides(RE::TESObjectREFR* reference) = 0;
	virtual void SetSkinProperty(RE::TESObjectREFR* refr, bool isFemale, bool firstPerson, std::uint32_t slotMask, OverrideVariant* value, bool immediate) = 0;
	virtual void GetSkinProperty(RE::TESObjectREFR* refr, bool firstPerson, std::uint32_t slotMask, OverrideVariant* value) = 0;


	virtual void VisitNodes(RE::TESObjectREFR* refr, std::function<void(SKEEFixedString, OverrideVariant&)> functor) = 0;
	virtual void VisitSkin(RE::TESObjectREFR* refr, bool isFemale, bool firstPerson, std::function<void(std::uint32_t, OverrideVariant&)> functor) = 0;
	virtual void VisitStrings(std::function<void(SKEEFixedString)> functor) = 0;

private:
	ActorRegistrationMapHolder armorData;
	NodeRegistrationMapHolder nodeData;
	WeaponRegistrationMapHolder weaponData;
	SkinRegistrationMapHolder skinData;

	// Inherited via IAddonAttachmentInterface
	virtual void OnAttach(RE::TESObjectREFR* refr, RE::TESObjectARMO* armor, RE::TESObjectARMA* addon, RE::NiAVObject* object, bool isFirstPerson, RE::NiNode* skeleton, RE::NiNode* root) = 0;
};