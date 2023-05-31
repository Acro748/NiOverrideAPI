#pragma once

#include "IPluginInterface.h"

#include <set>
#include <string>

using namespace RE;
class OverlayHolder : public std::set<std::uint64_t>
{
public:
};

class OverlayInterface
	: public IPluginInterface
	, public IAddonAttachmentInterface
{
public:
	virtual std::uint32_t GetVersion() = 0;

	virtual void Save(SKSE::SerializationInterface* intfc, std::uint32_t kVersion) = 0;
	virtual bool Load(SKSE::SerializationInterface* intfc, std::uint32_t kVersion) = 0;
	virtual void Revert() = 0;

	virtual bool HasOverlays(TESObjectREFR* reference) = 0;
	virtual void AddOverlays(TESObjectREFR* reference) = 0;
	virtual void RemoveOverlays(TESObjectREFR* reference) = 0;
	virtual void RevertOverlays(TESObjectREFR* reference, bool resetDiffuse) = 0;
	virtual void RevertOverlay(TESObjectREFR* reference, BSFixedString nodeName, std::uint32_t armorMask, std::uint32_t addonMask, bool resetDiffuse) = 0;

	virtual void EraseOverlays(TESObjectREFR* reference) = 0;

	virtual void RevertHeadOverlays(TESObjectREFR* reference, bool resetDiffuse) = 0;
	virtual void RevertHeadOverlay(TESObjectREFR* reference, BSFixedString nodeName, std::uint32_t partType, std::uint32_t shaderType, bool resetDiffuse) = 0;

	virtual void SetupOverlay(std::uint32_t primaryCount, const char* primaryPath, const char* primaryNode, std::uint32_t secondaryCount, const char* secondaryPath, const char* secondaryNode, TESObjectREFR* refr, NiNode* boneTree, NiAVObject* resultNode) = 0;

	virtual void UninstallOverlay(const char* nodeName, TESObjectREFR* refr, NiNode* parent) = 0;
	virtual void InstallOverlay(const char* nodeName, const char* path, TESObjectREFR* refr, BSGeometry* source, NiNode* destination, BGSTextureSet* textureSet = NULL) = 0;
	virtual void ResetOverlay(const char* nodeName, TESObjectREFR* refr, BSGeometry* source, NiNode* destination, BGSTextureSet* textureSet = NULL, bool resetDiffuse = false) = 0; // Re-applies the skin's textures

	virtual std::string& GetDefaultTexture() = 0;
	virtual void SetDefaultTexture(const std::string& newTexture) = 0;

	// Relinks an overlay node by name to the new source
	virtual void RelinkOverlay(const char* nodeName, TESObjectREFR* refr, BSGeometry* source, NiNode* skeleton) = 0;

	// Relinks structured name of overlays to new source
	virtual void RelinkOverlays(std::uint32_t primaryCount, const char* primaryNode, std::uint32_t secondaryCount, const char* secondaryNode, TESObjectREFR* refr, NiNode* boneTree, NiAVObject* resultNode) = 0;

	// Builds default overlays
	virtual void BuildOverlays(std::uint32_t armorMask, std::uint32_t addonMask, TESObjectREFR* refr, NiNode* boneTree, NiAVObject* resultNode) = 0;

	// Relinks default overlays
	virtual void RebuildOverlays(std::uint32_t armorMask, std::uint32_t addonMask, TESObjectREFR* refr, NiNode* boneTree, NiAVObject* resultNode) = 0;

private:
	std::string defaultTexture;
	OverlayHolder overlays;

	// Inherited via IAddonAttachmentInterface
	virtual void OnAttach(TESObjectREFR* refr, TESObjectARMO* armor, TESObjectARMA* addon, NiAVObject* object, bool isFirstPerson, NiNode* skeleton, NiNode* root) override = 0;
};
