#include "ResourceMgr.h"
#include "Shader.h"
#include "Texture.h"
#include "Audio/Audio.h"
#include "ColorConsole.h"
#include "Game/Song.h"

#include <iostream>


ResourceManager* ResourceManager::mpInstance;

void ResourceManager::Initialize()
{
	mAllImporters["shader"] = std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Shader>());
	mAllImporters["png"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Texture>());
	mAllImporters["jpg"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Texture>());
	mAllImporters["jpeg"]	= std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Texture>());
	mAllImporters["wav"] = std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Audio>());
	mAllImporters["mp3"] = std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Audio>());
	mAllImporters["ogg"] = std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Audio>());
	mAllImporters["ssc"] = std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Song>());
	mAllImporters["sm"] = std::shared_ptr<IResourceImporterBase>(new ResourceImporter<Song>());
}

void ResourceManager::Shutdown()
{
	mAllResources.clear();
	mAllImporters.clear();
}

const std::string ResourceManager::GetExtension(const std::string& filePath) const
{
	size_t pos = filePath.find_last_of('.');
	if (pos == std::string::npos)
		PrintWarning("File " + filePath + " has no extension.");
	return filePath.substr(pos + 1);
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
	std::cout << "[DEBUG] Deleted Resource Importer." << std::endl;
}