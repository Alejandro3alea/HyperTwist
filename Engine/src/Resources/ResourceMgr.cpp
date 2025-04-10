#include "ResourceMgr.h"
#include "Audio/Audio.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Misc/ColorConsole.h"
#include "Graphics/Font.h"
#include "Game/Song.h"
#include "Utils/StringUtils.h"

#include <iostream>
#include <filesystem>

ResourceManager* ResourceManager::mpInstance;

void ResourceManager::Initialize()
{
	mAllImporters["shader"] = std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Shader>());
	mAllImporters["png"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Texture>());
	mAllImporters["jpg"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Texture>());
	mAllImporters["jpeg"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Texture>());
	mAllImporters["wav"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Audio>());
	mAllImporters["mp3"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Audio>());
	mAllImporters["ogg"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Audio>());
	mAllImporters["ssc"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Song>());
	mAllImporters["sm"]		= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Song>());
	mAllImporters["smd"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Song>());
	mAllImporters["scd"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Song>());
	mAllImporters["ttf"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Font>());

	mBasePath = std::filesystem::current_path().string();
}

void ResourceManager::LoadDefaultAssets()
{
	mDefaultResources[typeid(Shader).name()] = Load<Shader>("/engine/shaders/Quad.shader");
	mDefaultResources[typeid(Texture).name()] = Load<Texture>("/engine/texture/Default.png");
	mDefaultResources[typeid(Audio).name()] = Load<Audio>("/engine/audio/Default.mp3");
	mDefaultResources[typeid(Font).name()] = Load<Font>("/engine/fonts/Cousine-Regular.ttf");
}

void ResourceManager::Shutdown()
{
	mDefaultResources.clear();
	mAllResources.clear();
	mAllImporters.clear();
}

const std::string ResourceManager::GetExtension(const std::string& filePath) const
{
	size_t pos = filePath.find_last_of('.');
	if (pos == std::string::npos)
		PrintWarning("File " + filePath + " has no extension.");

	return ToLower(filePath.substr(pos + 1));
}

const std::string ResourceManager::GetResourceName(const std::string& filePath) const
{
	size_t extensionLoc = filePath.find_last_of('.');
	size_t directoryLoc = filePath.find_last_of('\\');

	size_t start = (directoryLoc != std::string::npos) ? directoryLoc + 1 : 0;
	size_t end = (extensionLoc != std::string::npos) ? extensionLoc : filePath.length();

	std::string result;
	for (size_t i = start; i < end; i++)
		result.push_back(filePath[i]);

	return result;
}

const std::string ResourceManager::GetPathWithoutExtension(const std::string& filePath) const
{
	size_t extensionLoc = filePath.find_last_of('.');
	size_t end = (extensionLoc != std::string::npos) ? extensionLoc : filePath.length();
	return filePath.substr(0, extensionLoc);
}

void ResourceManager::ReloadShaders()
{
	const std::shared_ptr<IResourceImporterBase> Importer = mAllImporters["shader"];

	for (auto& CurrResource : mAllResources)
	{
		const std::string Path = CurrResource.first;
		const std::string Ext = GetExtension(Path);

		if (Ext == "shader")
		{
			Shader NewShader(Path);
			Shader* CurrShader = dynamic_cast<Resource<Shader>*>(CurrResource.second.get())->get();
			std::swap(NewShader.mID, CurrShader->mID);
		}
	}
}

IResourceImporterBase::~IResourceImporterBase()
{
	PrintDebug("Deleted Resource Importer.");
}