#pragma once

class IPluginInterface
{
public:
	IPluginInterface() { };
	virtual ~IPluginInterface() { };

	virtual std::uint32_t GetVersion() = 0;
	virtual void Revert() = 0;
};

class IInterfaceMap 
{
public:
	virtual IPluginInterface * QueryInterface(const char * name) = 0;
	virtual bool AddInterface(const char * name, IPluginInterface * pluginInterface) = 0;
	virtual IPluginInterface * RemoveInterface(const char * name) = 0;
};

struct InterfaceExchangeMessage
{
	enum
	{
		kMessage_ExchangeInterface = 0x9E3779B9
	};

	IInterfaceMap * interfaceMap = nullptr;
};

class IAddonAttachmentInterface
{
public:
	virtual void OnAttach(RE::TESObjectREFR* refr, RE::TESObjectARMO* armor, RE::TESObjectARMA* addon, RE::NiAVObject* object, bool isFirstPerson, RE::NiNode* skeleton, RE::NiNode* root) = 0;
};

class IBodyMorphInterface : public IPluginInterface
{
public:
	class MorphKeyVisitor
	{
	public:
		virtual void Visit(const char*, float) = 0;
	};

	class StringVisitor
	{
	public:
		virtual void Visit(const char*) = 0;
	};

	class ActorVisitor
	{
	public:
		virtual void Visit(RE::TESObjectREFR*) = 0;
	};

	class MorphValueVisitor
	{
	public:
		virtual void Visit(RE::TESObjectREFR*, const char*, const char*, float) = 0;
	};

	class MorphVisitor
	{
	public:
		virtual void Visit(RE::TESObjectREFR*, const char*) = 0;
	};

	virtual void SetMorph(RE::TESObjectREFR* actor, const char* morphName, const char* morphKey, float relative) = 0;
	virtual float GetMorph(RE::TESObjectREFR* actor, const char* morphName, const char* morphKey) = 0;
	virtual void ClearMorph(RE::TESObjectREFR* actor, const char* morphName, const char* morphKey) = 0;

	virtual float GetBodyMorphs(RE::TESObjectREFR* actor, const char* morphName) = 0;
	virtual void ClearBodyMorphNames(RE::TESObjectREFR* actor, const char* morphName) = 0;

	virtual void VisitMorphs(RE::TESObjectREFR* actor, MorphVisitor& visitor) = 0;
	virtual void VisitKeys(RE::TESObjectREFR* actor, const char* name, MorphKeyVisitor& visitor) = 0;
	virtual void VisitMorphValues(RE::TESObjectREFR* actor, MorphValueVisitor& visitor) = 0;

	virtual void ClearMorphs(RE::TESObjectREFR* actor) = 0;

	virtual void ApplyVertexDiff(RE::TESObjectREFR* refr, RE::NiAVObject* rootNode, bool erase = false) = 0;

	virtual void ApplyBodyMorphs(RE::TESObjectREFR* refr, bool deferUpdate = true) = 0;
	virtual void UpdateModelWeight(RE::TESObjectREFR* refr, bool immediate = false) = 0;

	virtual void SetCacheLimit(std::size_t limit) = 0;
	virtual bool HasMorphs(RE::TESObjectREFR* actor) = 0;
	virtual std::uint32_t EvaluateBodyMorphs(RE::TESObjectREFR* actor) = 0;

	virtual bool HasBodyMorph(RE::TESObjectREFR* actor, const char* morphName, const char* morphKey) = 0;
	virtual bool HasBodyMorphName(RE::TESObjectREFR* actor, const char* morphName) = 0;
	virtual bool HasBodyMorphKey(RE::TESObjectREFR* actor, const char* morphKey) = 0;
	virtual void ClearBodyMorphKeys(RE::TESObjectREFR* actor, const char* morphKey) = 0;
	virtual void VisitStrings(StringVisitor& visitor) = 0;
	virtual void VisitActors(ActorVisitor& visitor) = 0;
	virtual std::size_t ClearMorphCache() = 0;
};

class INiTransformInterface : public IPluginInterface
{
public:
	struct Position
	{
		float x, y, z;
	};
	struct Rotation //y, z, x
	{
		float heading, attitude, bank;
	};

	// Visits all overrides within a set
	class NodeVisitor
	{
	public:
		virtual bool VisitPosition(const char* node, const char* key, Position& position) = 0;
		virtual bool VisitRotation(const char* node, const char* key, Rotation& rotation) = 0;
		virtual bool VisitScale(const char* node, const char* key, float scale) = 0;
		virtual bool VisitScaleMode(const char* node, const char* key, std::uint32_t scaleMode) = 0;
	};

	virtual bool HasNodeTransformPosition(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name) = 0;
	virtual bool HasNodeTransformRotation(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name) = 0;
	virtual bool HasNodeTransformScale(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name) = 0;
	virtual bool HasNodeTransformScaleMode(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name) = 0;

	virtual void AddNodeTransformPosition(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name, Position& position) = 0; // X,Y,Z
	virtual void AddNodeTransformRotation(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name, Rotation& rotation) = 0; // Euler angles
	virtual void AddNodeTransformScale(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name, float scale) = 0;
	virtual void AddNodeTransformScaleMode(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name, std::uint32_t scaleMode) = 0;

	virtual Position GetNodeTransformPosition(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name) = 0;
	virtual Rotation GetNodeTransformRotation(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name) = 0;
	virtual float GetNodeTransformScale(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name) = 0;
	virtual std::uint32_t GetNodeTransformScaleMode(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name) = 0;

	virtual bool RemoveNodeTransformPosition(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name) = 0;
	virtual bool RemoveNodeTransformRotation(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name) = 0;
	virtual bool RemoveNodeTransformScale(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name) = 0;
	virtual bool RemoveNodeTransformScaleMode(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node, const char* name) = 0;

	virtual bool RemoveNodeTransform(RE::TESObjectREFR* refr, bool firstPerson, bool isFemale, const char* node, const char* name) = 0;
	virtual void RemoveAllReferenceTransforms(RE::TESObjectREFR* refr) = 0;

	virtual bool GetOverrideNodeTransform(RE::TESObjectREFR* refr, bool firstPerson, bool isFemale, const char* node, const char* name, std::uint16_t key, RE::NiTransform* result) = 0;

	virtual void UpdateNodeAllTransforms(RE::TESObjectREFR* ref) = 0;

	virtual void VisitNodes(RE::TESObjectREFR* refr, bool firstPerson, bool isFemale, NodeVisitor& visitor) = 0;
	virtual void UpdateNodeTransforms(RE::TESObjectREFR* ref, bool firstPerson, bool isFemale, const char* node) = 0;
};

