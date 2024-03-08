#ifndef __SIZE_LAYOUT_MANAGER_H_INCLUDED__
#define __SIZE_LAYOUT_MANAGER_H_INCLUDED__

#include <list>

struct SizingMethod
{
  SizingMethod (bool _stick_to_left   = true,
                bool _stick_to_top    = true,
                bool _stick_to_right  = true,
                bool _stick_to_bottom = true)
  {
      stick_to_left   = _stick_to_left;
      stick_to_right  = _stick_to_right;
      stick_to_top    = _stick_to_top;
      stick_to_bottom = _stick_to_bottom;
  }
  bool stick_to_left;
  bool stick_to_top;
  bool stick_to_right;
  bool stick_to_bottom;
};

class CSizeLayoutManager
{
public:
    class ControlLayout
    {
    public:
        ControlLayout (CWnd* parent, UINT id, const SizingMethod& _sizing_method) {
			id_ = id;
            wnd_ = 0;
            sizing_method_ = _sizing_method;

            CRect control_rect;
            CRect window_rect;

			parent->GetDlgItem(id_)->GetWindowRect (&control_rect);
            parent->ScreenToClient (&control_rect);
            parent->GetClientRect (&window_rect);

            margins_.left = control_rect.left;
            margins_.top = control_rect.top;
            margins_.right = window_rect.Width () - control_rect.right;
            margins_.bottom = window_rect.Height () - control_rect.bottom;
        }
        ControlLayout (CWnd* parent, CWnd* _wnd, const SizingMethod& _sizing_method) {
			id_ = 0;
            wnd_ = _wnd;
            sizing_method_ = _sizing_method;

            CRect control_rect;
            CRect window_rect;
            CWnd* parent_ = wnd_->GetParent ();

            wnd_->GetWindowRect (&control_rect);
            parent_->ScreenToClient (&control_rect);
            parent_->GetClientRect (&window_rect);

            margins_.left = control_rect.left;
            margins_.top = control_rect.top;
            margins_.right = window_rect.Width () - control_rect.right;
            margins_.bottom = window_rect.Height () - control_rect.bottom;
        }
        
		void Update (CWnd* parent) {
			if(id_) {
				wnd_ = parent->GetDlgItem(id_);
			}
			if (! IsWindow (wnd_->GetSafeHwnd ())) {
                return;
			}

            CRect rect, new_margins;
            CRect window_rect;

			wnd_->GetWindowRect (&rect);
            parent->ScreenToClient (&rect);
            parent->GetClientRect (&window_rect);

            new_margins.left = rect.left;
            new_margins.top = rect.top;
            new_margins.right = window_rect.Width () - rect.right;
            new_margins.bottom = window_rect.Height () - rect.bottom;

            if (sizing_method_.stick_to_right &&
                new_margins.right != margins_.right)
            {
                new_margins.left += new_margins.right - margins_.right;
                new_margins.right = margins_.right;
            }

            if (sizing_method_.stick_to_bottom &&
                new_margins.bottom != margins_.bottom)
            {
                new_margins.top += new_margins.bottom - margins_.bottom;
                new_margins.bottom = margins_.bottom;
            }

            if (sizing_method_.stick_to_left &&
                new_margins.left != margins_.left)
            {
                new_margins.left = margins_.left;
            }

            if (sizing_method_.stick_to_top &&
                new_margins.top != margins_.top)
            {
                new_margins.top = margins_.top;
            }

            rect.left = new_margins.left;
            rect.top = new_margins.top;
            rect.right = window_rect.Width () - new_margins.right;
            rect.bottom = window_rect.Height () - new_margins.bottom;

            wnd_->MoveWindow (&rect, 0);
        }
    private:
        CWnd* wnd_;
		UINT id_;
        SizingMethod sizing_method_;
        CRect margins_;
    };

    CSizeLayoutManager ()
    : initialized_ (false)
    , wnd_ (0)
    {
    }

    void add (bool to_left, bool to_top, bool to_right, bool to_bottom, UINT id) {
        xassert (initialized_ && wnd_);
        if (CDialog* dlg = static_cast<CDialog*>(wnd_)) {
            CWnd* control = dlg->GetDlgItem (id);
            xassert ("Layout Manager: Unable to register such control" && control && IsWindow (control->GetSafeHwnd ()));
            controls_.push_back (ControlLayout (wnd_, id, SizingMethod(to_left, to_top, to_right, to_bottom)));
        }
    }

    void add (bool to_left, bool to_top, bool to_right, bool to_bottom, CWnd* control) {
        xassert ("Layout Manager: Unable to register such control" && control && IsWindow (control->GetSafeHwnd ()));
        controls_.push_back (ControlLayout (control->GetParent(), control, SizingMethod(to_left, to_top, to_right, to_bottom)));
    }

    void init (CWnd* wnd) {
		controls_.clear();
        wnd_ = wnd;
        CRect rt;
        wnd_->GetWindowRect (&rt);
        initial_size_ = rt.Size();
        size_ = initial_size_;
        initialized_ = true;
    }
	void reset() {
		initialized_ = false;
		wnd_ = 0;
		controls_.clear();
	}

    CSize initialSize () const {
        return initial_size_;
    }

    void onSize (unsigned int _cx, unsigned int _cy) {
        CSize _size (_cx, _cy);

        if (size_ == _size)
            return;

        ControlsArray::iterator it;

        for (it = controls_.begin (); it != controls_.end (); ++it) {
            it->Update(wnd_);
        }
        size_ = _size;
        if (wnd_ && ::IsWindow (wnd_->GetSafeHwnd())) {
            wnd_->RedrawWindow (0, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
        }
    }

    bool isInitialized() const { return initialized_; }
private:
  typedef std::list<ControlLayout> ControlsArray;
  ControlsArray controls_;
  CWnd* wnd_;
  CSize size_;
  CSize initial_size_;
  bool initialized_;
};
#endif
