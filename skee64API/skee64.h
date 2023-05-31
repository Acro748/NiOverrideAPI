#pragma once

#include "SafeDataHolder.h"
#include "NiTransformInterface.h"
#include "OverlayInterface.h"
#include "OverrideInterface.h"
#include "TintMaskInterface.h"

using namespace RE;

class BodyMorphInterfaceAPI;
class NiTransformInterfaceAPI;
class OverlayInterfaceAPI;
class TintMaskInterfaceAPI;
class SKEE64API {
public:
	SKEE64API() {};
	~SKEE64API() {};
	
	inline InterfaceExchangeMessage& GetExchangeMessage() { return exchangeMessage; }

	inline bool IsSKEE64Loaded() {
		if (!SKEE64_Check) {
			SKEE64 = GetModuleHandle("skee64.dll");
		}
		return SKEE64;
	}

	inline bool SKEE64load() {
		if (!SKEE64load_Check.exchange(true))
		{
			if (IsSKEE64Loaded())
			{
				const auto messagingInterface = SKSE::GetMessagingInterface();
				if (messagingInterface)
					messagingInterface->Dispatch(InterfaceExchangeMessage::kMessage_ExchangeInterface, &GetExchangeMessage(), 0, "SKEE");
			}
		}
		return GetExchangeMessage().interfaceMap ? true : false;
	}

	BodyMorphInterfaceAPI* GetBodyMorphInterfaceAPI() { return reinterpret_cast<BodyMorphInterfaceAPI*>(this); }
	NiTransformInterfaceAPI* GetNiTransformInterfaceAPI() { return reinterpret_cast<NiTransformInterfaceAPI*>(this); }
	OverlayInterfaceAPI* GetOverlayInterfaceAPI() { return reinterpret_cast<OverlayInterfaceAPI*>(this); }
	TintMaskInterfaceAPI* GetTintMaskInterfaceAPI() { return reinterpret_cast<TintMaskInterfaceAPI*>(this); }

protected:

private:
	bool SKEE64 = false;
	bool SKEE64_Check = false;
	std::atomic<bool> SKEE64load_Check = false;

	InterfaceExchangeMessage exchangeMessage;
};



class BodyMorphInterfaceAPI :
	public SKEE64API
{
	IPluginInterface* bodymorphInterface = nullptr;
	const std::string interfaceName_ = "BodyMorph";
public:
	BodyMorphInterfaceAPI(std::string interfaceName = "BodyMorph") : interfaceName_(interfaceName) {};
	~BodyMorphInterfaceAPI() {};

	inline bool BodyMorphInterfaceload() {
		if (!bodymorphInterface && SKEE64load())
		{
			bodymorphInterface = GetExchangeMessage().interfaceMap->QueryInterface("BodyMorph");
		}
		return bodymorphInterface ? true : false;
	}

	inline std::uint32_t GetVersion() {
		if (BodyMorphInterfaceload())
		{
			return bodymorphInterface->GetVersion();
		}
		return 0;
	}

	inline IBodyMorphInterface* GetIBodyMorphInterface() {
		if (BodyMorphInterfaceload())
		{
			if (bodymorphInterface->GetVersion() == 4)
				return static_cast<IBodyMorphInterface*>(bodymorphInterface);
		}
		return nullptr;
	}

	void SetMorph(TESObjectREFR* actor, const char* morphName, const char* morphKey, float relative) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->SetMorph(actor, morphName, morphKey, relative);
	}
	float GetMorph(TESObjectREFR* actor, const char* morphName, const char* morphKey) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->GetMorph(actor, morphName, morphKey);
	}
	void ClearMorph(TESObjectREFR* actor, const char* morphName, const char* morphKey) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->ClearMorph(actor, morphName, morphKey);
	}

	float GetBodyMorphs(TESObjectREFR* actor, const char* morphName) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->GetBodyMorphs(actor, morphName);
	}
	void ClearBodyMorphNames(TESObjectREFR* actor, const char* morphName) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->ClearBodyMorphNames(actor, morphName);
	}

	void ClearMorphs(TESObjectREFR* actor) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->ClearMorphs(actor);
	}

	void ApplyVertexDiff(TESObjectREFR* refr, NiAVObject* object, bool erase = false) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->ApplyVertexDiff(refr, object, erase);
	}

	void ApplyBodyMorphs(TESObjectREFR* refr, bool deferUpdate = true) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->ApplyBodyMorphs(refr, deferUpdate);
	}
	void UpdateModelWeight(TESObjectREFR* refr, bool immediate = false) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->UpdateModelWeight(refr, immediate);
	}

	bool HasMorphs(TESObjectREFR* actor) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->HasMorphs(actor);
	}
	std::uint32_t BodyMorphsSize(TESObjectREFR* actor) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->EvaluateBodyMorphs(actor);
	}

	bool HasBodyMorph(TESObjectREFR* actor, const char* morphName, const char* morphKey) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->HasBodyMorph(actor, morphName, morphKey);
	}
	bool HasBodyMorphName(TESObjectREFR* actor, const char* morphName) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->HasBodyMorphName(actor, morphName);
	}
	bool HasBodyMorphKey(TESObjectREFR* actor, const char* morphKey) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->HasBodyMorphKey(actor, morphKey);
	}
	void ClearBodyMorphKeys(TESObjectREFR* actor, const char* morphKey) {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->ClearBodyMorphKeys(actor, morphKey);
	}

	size_t ClearMorphCache() {
		if (IBodyMorphInterface* bmi = GetIBodyMorphInterface(); bmi)
			bmi->ClearMorphCache();
	}
};

class NiTransformInterfaceAPI :
	public SKEE64API
{
	IPluginInterface* niTransformInterface = nullptr;
	const std::string interfaceName_ = "NiTransform";
	const std::string keyName_ = "internal";
public:
	NiTransformInterfaceAPI(std::string keyName = "internal", std::string interfaceName = "NiTransform") : keyName_(keyName), interfaceName_(interfaceName) {};
	~NiTransformInterfaceAPI() {};
	
	inline const char* GetKeyName() { return keyName_.c_str(); }

	inline bool NiTransformInterfaceload() {
		if (!niTransformInterface && SKEE64load())
		{
			niTransformInterface = GetExchangeMessage().interfaceMap->QueryInterface(interfaceName_.c_str());
		}
		return niTransformInterface ? true : false;
	}

	inline std::uint32_t GetVersion() {
		if (NiTransformInterfaceload())
		{
			return niTransformInterface->GetVersion();
		}
		return 0;
	}

	inline NiTransformInterface* GetNiTransformInterface() {
		if (NiTransformInterfaceload())
		{
			if (niTransformInterface->GetVersion() == 2)
				return static_cast<NiTransformInterface*>(niTransformInterface);
		}
		return nullptr;
	}
	inline INiTransformInterface* GetINiTransformInterface() {
		if (NiTransformInterfaceload())
		{
			if (niTransformInterface->GetVersion() == 3)
				return static_cast<INiTransformInterface*>(niTransformInterface);
		}
		return nullptr;
	}

	bool HasNodeTransformPosition(TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			std::uint16_t param = OverrideVariant::kParam_NodeTransformPosition;
			NiTransform result;
			return nti->GetOverrideNodeTransform(ref, firstPerson, isFemale, node, GetKeyName(), param, &result);
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			return nti->HasNodeTransformPosition(ref, firstPerson, isFemale, node, GetKeyName());
		}
		return false;
	}
	bool HasNodeTransformRotation(TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			std::uint16_t param = OverrideVariant::kParam_NodeTransformRotation;
			NiTransform result;
			return nti->GetOverrideNodeTransform(ref, firstPerson, isFemale, node, GetKeyName(), param, &result);
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			return nti->HasNodeTransformRotation(ref, firstPerson, isFemale, node, GetKeyName());
		}
		return false;
	}
	bool HasNodeTransformScale(TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			std::uint16_t param = OverrideVariant::kParam_NodeTransformScale;
			NiTransform result;
			return nti->GetOverrideNodeTransform(ref, firstPerson, isFemale, node, GetKeyName(), param, &result);
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			return nti->HasNodeTransformScale(ref, firstPerson, isFemale, node, GetKeyName());
		}
		return false;
	}
	bool HasNodeTransformScaleMode(TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			std::uint16_t param = OverrideVariant::kParam_NodeTransformScaleMode;
			OverrideVariant overrideVariant = nti->GetOverrideNodeValue(ref, firstPerson, isFemale, node, name, param, 0);
			if (overrideVariant.type == OverrideVariant::kType_Int && overrideVariant.key == OverrideVariant::kParam_NodeTransformScale)
			{
				return true;
			}
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			return nti->HasNodeTransformScaleMode(ref, firstPerson, isFemale, node, GetKeyName());
		}
		return false;
	}

	void SetNodeTransform(TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, NiPoint3 value = NiPoint3()) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			std::uint16_t param = OverrideVariant::kParam_NodeTransformPosition;
			if (value != NiPoint3())
			{
				float pos[3] = { value.x, value.y, value.z };
				OverrideVariant posV[3];
				for (std::uint32_t i = 0; i < 3; i++) {
					posV[i].SetFloat(param, i, pos[i]);
					nti->AddNodeTransform(ref, firstPerson, isFemale, node, GetKeyName(), posV[i]);
				}
			}
			else
			{
				for (std::uint32_t i = 0; i < 3; i++)
					nti->RemoveNodeTransformComponent(ref, firstPerson, isFemale, node, GetKeyName(), param, i);
			}
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			if (value != NiPoint3())
			{
				INiTransformInterface::Position pos = { value.x, value.y, value.z };
				nti->AddNodeTransformPosition(ref, firstPerson, isFemale, node, GetKeyName(), pos);
			}
			else
			{
				nti->RemoveNodeTransformPosition(ref, firstPerson, isFemale, node, GetKeyName());
			}
		}
	}
	void SetNodeTransform(TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, NiMatrix3 value = NiMatrix3()) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			std::uint16_t param = OverrideVariant::kParam_NodeTransformRotation;
			if (value != NiMatrix3())
			{
				OverrideVariant rotV[9];
				for (std::uint32_t i = 0; i < 3; i++)
				{
					for (std::uint32_t j = 0; j < 3; j++)
					{
						rotV[(i * 3) + j].SetFloat(param, (i * 3) + j, value.entry[i][j]);
						nti->AddNodeTransform(ref, firstPerson, isFemale, node, GetKeyName(), rotV[(i * 3) + j]);
					}
				}
			}
			else
			{
				for (std::uint32_t i = 0; i < 9; i++)
					nti->RemoveNodeTransformComponent(ref, firstPerson, isFemale, node, GetKeyName(), param, i);
			}
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			if (value != NiMatrix3())
			{
				NiPoint3 rot = util.GetEulerAngles(value);
				INiTransformInterface::Rotation rotV = { rot.y * util.toDegree, rot.z * util.toDegree, rot.x * util.toDegree };
				nti->AddNodeTransformRotation(ref, firstPerson, isFemale, node, GetKeyName(), rotV);
			}
			else
			{
				nti->RemoveNodeTransformRotation(ref, firstPerson, isFemale, node, GetKeyName());
			}
		}
	}
	void SetNodeTransform(TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, float value = 1.0f) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			std::uint16_t param = OverrideVariant::kParam_NodeTransformScale;
			if (value != 1.0f)
			{
				OverrideVariant scale;
				scale.SetFloat(param, 0, value);
				nti->AddNodeTransform(ref, firstPerson, isFemale, node, GetKeyName(), scale);
			}
			else
			{
				nti->RemoveNodeTransformComponent(ref, firstPerson, isFemale, node, GetKeyName(), param, 0);
			}
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			if (value != 1.0f)
			{
				nti->AddNodeTransformScale(ref, firstPerson, isFemale, node, GetKeyName(), value);
			}
			else
			{
				nti->RemoveNodeTransformScale(ref, firstPerson, isFemale, node, GetKeyName());
			}
		}
	}
	void SetNodeTransform(TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, std::uint32_t ScaleMode) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			std::uint16_t param = OverrideVariant::kParam_NodeTransformScaleMode;
			OverrideVariant scaleModeVar;
			scaleModeVar.SetInt(param, 0, ScaleMode);
			nti->AddNodeTransform(ref, firstPerson, isFemale, node, GetKeyName(), scaleModeVar);
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			nti->AddNodeTransformScaleMode(ref, firstPerson, isFemale, node, GetKeyName(), ScaleMode);
		}
	}
	void RemoveNodeTransform(TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			nti->RemoveNodeTransform(ref, firstPerson, isFemale, node, GetKeyName());
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			nti->RemoveNodeTransform(ref, firstPerson, isFemale, node, GetKeyName());
		}
	}
	void RemoveAllReferenceTransforms(TESObjectREFR* ref) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			nti->RemoveAllReferenceTransforms(ref);
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			nti->RemoveAllReferenceTransforms(ref);
		}
	}

	NiPoint3 GetNodeTransformPosition(TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			std::uint16_t param = OverrideVariant::kParam_NodeTransformPosition;
			NiTransform result;
			if (nti->GetOverrideNodeTransform(ref, firstPerson, isFemale, node, GetKeyName(), param, &result))
				return result.translate;
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			if (nti->HasNodeTransformPosition(ref, firstPerson, isFemale, node, GetKeyName()))
			{
				INiTransformInterface::Position pos = nti->GetNodeTransformPosition(ref, firstPerson, isFemale, node, GetKeyName());
				return NiPoint3(pos.x, pos.y, pos.z);
			}
		}
		return NiPoint3();
	}
	NiMatrix3 GetNodeTransformRotation(TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			std::uint16_t param = OverrideVariant::kParam_NodeTransformRotation;
			NiTransform result;
			if (nti->GetOverrideNodeTransform(ref, firstPerson, isFemale, node, GetKeyName(), param, &result))
				return result.rotate;
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			if (nti->HasNodeTransformRotation(ref, firstPerson, isFemale, node, GetKeyName()))
			{
				INiTransformInterface::Rotation rot = nti->GetNodeTransformRotation(ref, firstPerson, isFemale, node, GetKeyName());
				return util.SetEulerAngles(rot.bank * util.toRadian, rot.heading * util.toRadian, rot.attitude * util.toRadian);
			}
		}
		return NiMatrix3();
	}
	float GetNodeTransformScale(TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			std::uint16_t param = OverrideVariant::kParam_NodeTransformScale;
			NiTransform result;
			if (nti->GetOverrideNodeTransform(ref, firstPerson, isFemale, node, GetKeyName(), param, &result))
				return result.scale;
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			if (nti->HasNodeTransformScale(ref, firstPerson, isFemale, node, GetKeyName()))
			{
				return nti->GetNodeTransformScale(ref, firstPerson, isFemale, node, GetKeyName());
			}
		}
		return 1.0f;
	}
	std::uint32_t GetNodeTransformScaleMode(TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			std::uint16_t param = OverrideVariant::kParam_NodeTransformScaleMode;
			OverrideVariant overrideVariant = nti->GetOverrideNodeValue(ref, firstPerson, isFemale, node, GetKeyName(), param, 0);
			if (overrideVariant.type == OverrideVariant::kType_Int && overrideVariant.key == OverrideVariant::kParam_NodeTransformScale)
			{
				return overrideVariant.data.u;
			}
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			if (nti->HasNodeTransformScaleMode(ref, firstPerson, isFemale, node, GetKeyName()))
			{
				return nti->GetNodeTransformScaleMode(ref, firstPerson, isFemale, node, GetKeyName());
			}
		}
		return 0;
	}

	void UpdateNodeTransforms(TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			nti->UpdateNodeTransforms(ref, firstPerson, isFemale, node);
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			nti->UpdateNodeTransforms(ref, firstPerson, isFemale, node);
		}
	}
	void UpdateNodeAllTransforms(TESObjectREFR* ref) {
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			nti->UpdateNodeAllTransforms(ref);
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			nti->UpdateNodeAllTransforms(ref);
		}
	}

	void InsertBaseTransform(TESObjectREFR* ref, const char* nodeName, NiTransform transform = NiTransform()) {
		Actor* actor = skyrim_cast<Actor*>(ref);
		TESNPC* npc = actor ? actor->GetActorBase() : nullptr;
		if (!npc)
			return;
		logger::info("1");
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			logger::info("2");
			nti->InsertBaseTransform(nti->Impl_GetRootModelPath(ref, false, npc->GetSex()), nodeName, transform);
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			IPluginInterface* pif = static_cast<IPluginInterface*>(nti);
			NiTransformInterface* base_nti = static_cast<NiTransformInterface*>(pif);
			if (base_nti)
			{
				base_nti->InsertBaseTransform(base_nti->Impl_GetRootModelPath(ref, false, npc->GetSex()), nodeName, transform);
			}
		}
	}
	bool HasBaseTransform(TESObjectREFR* ref) {
		Actor* actor = skyrim_cast<Actor*>(ref);
		TESNPC* npc = actor ? actor->GetActorBase() : nullptr;
		if (!npc)
			return false;
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			return nti->HasBaseTransform(nti->Impl_GetRootModelPath(ref, false, npc->GetSex()));
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			IPluginInterface* pif = static_cast<IPluginInterface*>(nti);
			NiTransformInterface* base_nti = static_cast<NiTransformInterface*>(pif);
			if (base_nti)
			{
				return base_nti->HasBaseTransform(base_nti->Impl_GetRootModelPath(ref, false, npc->GetSex()));
			}
		}
		return false;
	}
	bool HasBaseTransform(TESObjectREFR* ref, const char* nodeName) {
		Actor* actor = skyrim_cast<Actor*>(ref);
		TESNPC* npc = actor ? actor->GetActorBase() : nullptr;
		if (!npc)
			return false;
		if (NiTransformInterface* nti = GetNiTransformInterface(); nti)
		{
			return nti->HasBaseTransform(nti->Impl_GetRootModelPath(ref, false, npc->GetSex()), nodeName);
		}
		else if (INiTransformInterface* nti = GetINiTransformInterface(); nti)
		{
			IPluginInterface* pif = static_cast<IPluginInterface*>(nti);
			NiTransformInterface* base_nti = static_cast<NiTransformInterface*>(pif);
			if (base_nti)
			{
				return base_nti->HasBaseTransform(base_nti->Impl_GetRootModelPath(ref, false, npc->GetSex()), nodeName);
			}
		}
		return false;
	}

private:
	struct util {
		const double pi = 3.14159265358979323846;
		const float toDegree = 180 / pi;
		const float toRadian = pi / 180;
		NiPoint3 GetEulerAngles(NiMatrix3 a_angles)
		{
			NiPoint3 a_point;
			if (a_angles.entry[1][0] > 0.998) { // singularity at north pole
				a_point.x = 0;
				a_point.y = atan2(a_angles.entry[0][2], a_angles.entry[2][2]);
				a_point.z = pi / 2;
			}
			else if (a_angles.entry[1][0] < -0.998) { // singularity at south pole
				a_point.x = 0;
				a_point.y = atan2(a_angles.entry[0][2], a_angles.entry[2][2]);
				a_point.z = -pi / 2;
			}
			else {
				a_point.x = atan2(-a_angles.entry[1][2], a_angles.entry[1][1]);
				a_point.y = atan2(-a_angles.entry[2][0], a_angles.entry[0][0]);
				a_point.z = asin(a_angles.entry[1][0]);
			}
			return a_point;
		}
		NiMatrix3 SetEulerAngles(float a_point_x, float a_point_y, float a_point_z)
		{
			NiMatrix3 a_angles;
			double cb = cos(a_point_x);
			double sb = sin(a_point_x);
			double ch = cos(a_point_y);
			double sh = sin(a_point_y);
			double ca = cos(a_point_z);
			double sa = sin(a_point_z);

			a_angles.entry[0][0] = ch * ca;
			a_angles.entry[0][1] = sh * sb - ch * sa * cb;
			a_angles.entry[0][2] = ch * sa * sb + sh * cb;
			a_angles.entry[1][0] = sa;
			a_angles.entry[1][1] = ca * cb;
			a_angles.entry[1][2] = -ca * sb;
			a_angles.entry[2][0] = -sh * ca;
			a_angles.entry[2][1] = sh * sa * cb + ch * sb;
			a_angles.entry[2][2] = -sh * sa * sb + ch * cb;
			return a_angles;
		}
	} util;
};

class OverlayInterfaceAPI :
	public SKEE64API
{
	IPluginInterface* overlayInterface = nullptr;
	std::string interfaceName_ = "Overlay";
public:
	OverlayInterfaceAPI(std::string interfaceName) : interfaceName_(interfaceName) {};
	~OverlayInterfaceAPI() {};

	inline bool OverlayInterfaceload() {
		if (!overlayInterface && SKEE64load())
		{
			overlayInterface = GetExchangeMessage().interfaceMap->QueryInterface(interfaceName_.c_str());
		}
		return overlayInterface ? true : false;
	}

	inline std::uint32_t GetVersion() {
		if (OverlayInterfaceload())
		{
			return overlayInterface->GetVersion();
		}
		return 0;
	}

	inline OverlayInterface* GetOverlayInterface() {
		if (OverlayInterfaceload())
		{
			if (overlayInterface->GetVersion() == 1)
				return static_cast<OverlayInterface*>(overlayInterface);
		}
		return nullptr;
	}

	bool HasOverlays(TESObjectREFR* reference) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->HasOverlays(reference);
		}
	}
	void AddOverlays(TESObjectREFR* reference) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->AddOverlays(reference);
		}
	}
	void RemoveOverlays(TESObjectREFR* reference) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->RemoveOverlays(reference);
		}
	}
	void RevertOverlays(TESObjectREFR* reference, bool resetDiffuse) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->RevertOverlays(reference, resetDiffuse);
		}
	}
	void RevertOverlay(TESObjectREFR* reference, BSFixedString nodeName, std::uint32_t armorMask, std::uint32_t addonMask, bool resetDiffuse) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->RevertOverlay(reference, nodeName, armorMask, addonMask, resetDiffuse);
		}
	}

	void EraseOverlays(TESObjectREFR* reference) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->EraseOverlays(reference);
		}
	}

	void RevertHeadOverlays(TESObjectREFR* reference, bool resetDiffuse) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->RevertHeadOverlays(reference, resetDiffuse);
		}
	}
	void RevertHeadOverlay(TESObjectREFR* reference, BSFixedString nodeName, std::uint32_t partType, std::uint32_t shaderType, bool resetDiffuse) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->RevertHeadOverlay(reference, nodeName, partType, shaderType, resetDiffuse);
		}
	}

	void SetupOverlay(std::uint32_t primaryCount, const char* primaryPath, const char* primaryNode, std::uint32_t secondaryCount, const char* secondaryPath, const char* secondaryNode, TESObjectREFR* refr, NiNode* boneTree, NiAVObject* resultNode) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->SetupOverlay(primaryCount, primaryPath, primaryNode, secondaryCount, secondaryPath, secondaryNode, refr, boneTree, resultNode);
		}
	}

	void UninstallOverlay(const char* nodeName, TESObjectREFR* refr, NiNode* parent) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->UninstallOverlay(nodeName, refr, parent);
		}
	}
	void InstallOverlay(const char* nodeName, const char* path, TESObjectREFR* refr, BSGeometry* source, NiNode* destination, BGSTextureSet* textureSet = NULL) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->InstallOverlay(nodeName, path, refr, source, destination, textureSet);
		}
	}
	void ResetOverlay(const char* nodeName, TESObjectREFR* refr, BSGeometry* source, NiNode* destination, BGSTextureSet* textureSet = NULL, bool resetDiffuse = false) { // Re-applies the skin's textures
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->ResetOverlay(nodeName, refr, source, destination, textureSet, resetDiffuse);
		}
	}
	std::string& GetDefaultTexture() {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			return oi->GetDefaultTexture();
		}
		std::string result = "";
		return result;
	}
	void SetDefaultTexture(const std::string& newTexture) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->SetDefaultTexture(newTexture);
		}
	}

	// Relinks an overlay node by name to the new source
	void RelinkOverlay(const char* nodeName, TESObjectREFR* refr, BSGeometry* source, NiNode* skeleton) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->RelinkOverlay(nodeName, refr, source, skeleton);
		}
	}

	// Relinks structured name of overlays to new source
	void RelinkOverlays(std::uint32_t primaryCount, const char* primaryNode, std::uint32_t secondaryCount, const char* secondaryNode, TESObjectREFR* refr, NiNode* boneTree, NiAVObject* resultNode) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->RelinkOverlays(primaryCount, primaryNode, secondaryCount, secondaryNode, refr, boneTree, resultNode);
		}
	}

	// Builds default overlays
	void BuildOverlays(std::uint32_t armorMask, std::uint32_t addonMask, TESObjectREFR* refr, NiNode* boneTree, NiAVObject* resultNode) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->BuildOverlays(armorMask, addonMask, refr, boneTree, resultNode);
		}
	}

	// Relinks default overlays
	void RebuildOverlays(std::uint32_t armorMask, std::uint32_t addonMask, TESObjectREFR* refr, NiNode* boneTree, NiAVObject* resultNode) {
		if (OverlayInterface* oi = GetOverlayInterface(); oi)
		{
			oi->RebuildOverlays(armorMask, addonMask, refr, boneTree, resultNode);
		}
	}
};

class TintMaskInterfaceAPI :
	public SKEE64API
{
	IPluginInterface* tintMaskInterface = nullptr;
	const std::string interfaceName_ = "TintMask";
public:
	TintMaskInterfaceAPI(std::string interfaceName) : interfaceName_(interfaceName) {};
	~TintMaskInterfaceAPI() {};

	inline bool TintMaskInterfaceload() {
		if (!tintMaskInterface && SKEE64load())
		{
			tintMaskInterface = GetExchangeMessage().interfaceMap->QueryInterface(interfaceName_.c_str());
		}
		return tintMaskInterface ? true : false;
	}

	inline std::uint32_t GetVersion() {
		if (TintMaskInterfaceload())
		{
			return tintMaskInterface->GetVersion();
		}
		return 0;
	}

	inline TintMaskInterface* GetTintMaskInterface() {
		if (TintMaskInterfaceload())
		{
			if (tintMaskInterface->GetVersion() == 0)
				return static_cast<TintMaskInterface*>(tintMaskInterface);
		}
		return nullptr;
	}

	void ApplyMasks(TESObjectREFR* refr, bool isFirstPerson, TESObjectARMO* armor, TESObjectARMA* addon, NiAVObject* object, std::uint32_t flags, ItemAttributeDataPtr overrides, LayerFunctor layer = LayerFunctor()) {
		if (TintMaskInterface* tmi = GetTintMaskInterface(); tmi)
		{
			tmi->ApplyMasks(refr, isFirstPerson, armor, addon, object, flags, overrides, layer);
		}
	}
	void ManageTints() {
		if (TintMaskInterface* tmi = GetTintMaskInterface(); tmi)
		{
			tmi->ManageTints();
		}
	}
	void ReleaseTints() {
		if (TintMaskInterface* tmi = GetTintMaskInterface(); tmi)
		{
			tmi->ReleaseTints();
		}
	}
	void Revert() {
		if (TintMaskInterface* tmi = GetTintMaskInterface(); tmi)
		{
			tmi->Revert();
		}
	}

	bool IsDyeable(TESObjectARMO* armor) {
		if (TintMaskInterface* tmi = GetTintMaskInterface(); tmi)
		{
			tmi->IsDyeable(armor);
		}
	}

	void GetTemplateColorMap(TESObjectREFR* actor, TESObjectARMO* armor, std::map<std::int32_t, std::uint32_t>& colorMap) {
		if (TintMaskInterface* tmi = GetTintMaskInterface(); tmi)
		{
			tmi->GetTemplateColorMap(actor, armor, colorMap);
		}
	}
	void GetSlotTextureIndexMap(TESObjectREFR* actor, TESObjectARMO* armor, std::map<std::int32_t, std::uint32_t>& slotTextureIndexMap) {
		if (TintMaskInterface* tmi = GetTintMaskInterface(); tmi)
		{
			tmi->GetSlotTextureIndexMap(actor, armor, slotTextureIndexMap);
		}
	}

	void LoadMods() {
		if (TintMaskInterface* tmi = GetTintMaskInterface(); tmi)
		{
			tmi->LoadMods();
		}
	}
};
