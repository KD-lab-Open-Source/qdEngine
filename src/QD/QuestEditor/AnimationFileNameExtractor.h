#pragma once

/*!
 	Достает из объектов разного типа анимацию
*/
class AnimationFileNameExtractor
{
	AnimationFileNameExtractor(void);
	~AnimationFileNameExtractor(void);
private:
	static LPCTSTR extractFromStaticState(class qdGameObjectStateStatic const* p);
	static LPCTSTR extractFromInterfaceElementState(
											class qdInterfaceElementState const* p);
	static LPCTSTR extractFromInterfaceSave(class qdInterfaceSave const* p);
	static LPCTSTR extractFromInterfaceSlider(class qdInterfaceSlider const* p);
	static LPCTSTR extractFromInterfaceBackground(class qdInterfaceBackground const* p);
public:
	/// Определяет тип объекта и вызывает соответствующую функцию
	static LPCTSTR getFileName(qdNamedObjectBase const *pobj);
};
