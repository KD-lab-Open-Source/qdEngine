#ifndef __EFFECT_REFERENCE_H__
#define __EFFECT_REFERENCE_H__

#include "XMath\Colors.h"
#include "Serialization\SerializationTypes.h"
#include "Units\Object3dxInterface.h"
#include "EffectContainer.h"

/// ��������� �����������
class EffectAttribute
{
public:
	EffectAttribute();
	EffectAttribute(const EffectReference& effectReference, bool isCycled);
	virtual ~EffectAttribute(){ };

	bool isEmpty() const { return effectReference_.get() == 0; }

	bool legionColor() const { return legionColor_; }

	void serialize(Archive& ar);

	bool isCycled() const { return isCycled_; }
	bool stopImmediately() const { return stopImmediately_; }
	bool bindOrientation() const { return bindOrientation_; }

	bool onWaterSurface() const { return (waterPlacementMode_ == WATER_SURFACE); }

	bool switchOffUnderWater() const { return switchOffUnderWater_; }
	bool switchOffUnderLava() const { return switchOffUnderLava_; }
	bool switchOffByDay() const { return switchOffByDay_; }
	bool switchOffOnIce() const { return switchOffOnIce_; }
	bool switchOnOnIce() const { return switchOnIce_; }

	bool switchOffByInterface() const { return switchOffByInterface_; }

	bool ignoreFogOfWar() const { return ignoreFogOfWar_; }
	bool ignoreInvisibility() const { return ignoreInvisibility_; }
	bool ignoreDistanceCheck() const { return ignoreDistanceCheck_; }

	float scale() const { return scale_; }
	
	const EffectReference& effectReference() const { return effectReference_; }
	EffectKey* effect(float scale = -1.f, Color4c skin_color = Color4c(255,255,255,255)) const { return isEmpty() ? 0 : effectReference_->getEffect(scale > 0.f ? scale : scale_, skin_color); }

	enum WaterPlacementMode {
		/// ������� �� ���
		WATER_BOTTOM,
		/// ������� �� ����������� ����
		WATER_SURFACE
	};

protected:

	/// ����������� ������ ��� ���
	bool isCycled_;
	/// ��������
	bool stopImmediately_;

	/// ������������� ������ �� �������
	bool bindOrientation_;

	/// ���������� � ���� �������
	bool legionColor_;

	/// ��������� � ����
	bool switchOffUnderWater_;
	/// ��������� � ����
	bool switchOffUnderLava_;
	/// ��������� ����
	bool switchOffByDay_;
	/// ��������� �� ����
	bool switchOffOnIce_;
	/// �������� ������ �� ����
	bool switchOnIce_;
	/// true ���� ����� � ������ �����
	bool ignoreFogOfWar_;
	/// true ���� ����� �� ��������� �����
	bool ignoreInvisibility_;
	/// true ���� �� ������ ��������� ��� �������� ������
	bool ignoreDistanceCheck_;
	/// ��������� ������ � �����������
	bool switchOffByInterface_;

	/// ����� �������� ������� �� ����
	WaterPlacementMode waterPlacementMode_;

	/// ������� �������
	float scale_;

	/// ������ �� ������ �� ���������� ��������
	EffectReference effectReference_;
};

class EffectAttributeAttachable : public EffectAttribute
{
	bool onlyForActivePlayer_;
	///�������������� �� ���������� �� ������� �������
	bool scaleByModel_;

	bool needNodeName_;
	bool synchronizationWithModelAnimation_;
	/// ����������� ���������
	bool switchOffByAnimationChain_;
	Object3dxNode node_;

public:
	EffectAttributeAttachable(bool need_node_name = true);
	EffectAttributeAttachable(const EffectReference& effectReference, bool isCycled);

	bool onlyForActivePlayer() const { return onlyForActivePlayer_; }
	bool scaleByModel() const {return scaleByModel_;}
	int node() const { return node_; };
	bool isSynchronize() const {return synchronizationWithModelAnimation_;}
	bool switchOffByAnimationChain() const { return switchOffByAnimationChain_; }

	void serialize(Archive& ar);
};

#endif //__EFFECT_REFERENCE_H__
