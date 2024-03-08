#pragma once


namespace win_h{
	inline bool keyPressed(int keyID){
		return (GetAsyncKeyState(keyID)>>15 != 0);
	}
}