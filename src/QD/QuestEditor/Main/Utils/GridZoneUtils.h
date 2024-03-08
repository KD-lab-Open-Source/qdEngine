#pragma once

class qdGameScene;
class qdGridZone;

/// Призван содержать набор функций для работы с зонами
class GridZoneUtils
{
public:
	/// Перевыбирает все зоны сцены
	static void reselectZones(qdGameScene const& scene);
};
