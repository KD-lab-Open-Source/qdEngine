#pragma once

interface ITriggerProfList
{
	enum eShowHide{TPL_SHOW, TPL_HIDE};
	virtual bool Show(eShowHide e) const = 0;
	virtual bool IsVisible() const = 0;
	virtual bool Load() = 0;
	virtual bool Next() const = 0;
	virtual bool Prev() const = 0;
	virtual bool Activate() = 0;
};
