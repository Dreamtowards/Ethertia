
#pragma once


class ChunkLoader
{
public:
	
	ChunkLoader(std::string_view m_SaveDir);



	bool LoadWorldInfo(WorldInfo& worldinfo);

	void SaveWorldInfo(const WorldInfo& worldinfo);



	bool LoadChunk(Chunk& chunk);

	void SaveChunk(const Chunk& chunk);



private:

	std::string m_SaveDir;

	// GetFile_WorldInfo();
	// GetFile_Chunk();

};