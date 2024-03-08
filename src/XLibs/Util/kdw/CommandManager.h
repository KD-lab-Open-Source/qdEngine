#ifndef __COMMANDS_H_INCLUDED__
#define __COMMANDS_H_INCLUDED__

#include <string>
#include <vector>
#include "XTL/sigslot.h"
#include "HotkeyContext.h"

class Archive;
namespace kdw{

class CommandSignal : public ShareHandleBase{
public:
	virtual ~CommandSignal() {}
	virtual void invoke() = 0;
};

class CommandSignal0 : public CommandSignal, public sigslot::signal0{
public:
	CommandSignal0(){}
	void invoke(){ emit(); }
protected:
};

template<class Arg1>
class CommandSignal1 : public CommandSignal, public sigslot::signal1<Arg1>{
public:
	CommandSignal1(Arg1 arg1) : arg1_(arg1) {}
	void invoke(){ emit(arg1_); }
protected:
	Arg1 arg1_;
};

class CommandManager;
class KDW_API CommandProxy{
public:
	CommandProxy(CommandManager* commandManager, int templateIndex);
	~CommandProxy() {}

	void setSignal(CommandSignal* signal);

	CommandProxy& setAutoCheck(bool autoCheck);
	bool autoCheck() const;

	CommandProxy& check(bool checked = true);
	bool checked() const;

	CommandProxy& enable(bool enabled = true);
	bool enabled() const;

	template<class T>
	CommandProxy& connect(T* object, void(T::*method)()){
		CommandSignal0* signal = new CommandSignal0();
		signal->connect(object, method);
		setSignal(signal);
		return *this;
	}
	template<class T, class Arg1>
	CommandProxy& connect(T* object, void(T::*method)(Arg1), Arg1 arg1){
		CommandSignal1<Arg1>* signal = new CommandSignal1<Arg1>(arg1);
		signal->connect(object, method);
		setSignal(signal);
		return *this;
	}
protected:
	CommandManager* commandManager_;
	int templateIndex_;
};

class CommandHotkey{
public:
	CommandHotkey();
	sKey key() const{ return key_; }
	bool sticky() const{ return sticky_; }

	void serialize(Archive& ar);
protected:
	sKey key_;
	bool sticky_;
};

class CommandTemplate;
class CommandInstance : public ShareHandleBase, public sigslot::has_slots{
public:
	CommandInstance(int templateIndex);
	CommandTemplate& getTemplate();
	int templateIndex() const{ return templateIndex_; }

	void setSignal(CommandSignal* signal);

	void enable(bool enabled);
	bool enabled() const{ return enabled_; }
	
	void check(bool checked = true);
	bool checked() const{ return checked_; }

	void setAutoCheck(bool autoCheck){ autoCheck_ = autoCheck; }
	bool autoCheck() const{ return autoCheck_; }
	CommandSignal* signal() { return signal_; }
	CommandHotkey hotkey() const{ return hotkey_; }
	void setHotkey(CommandHotkey key){ hotkey_ = key; }
	sigslot::signal1<CommandInstance*>& signalStateChanged(){ return signalStateChanged_; }
protected:
	ShareHandle<CommandSignal> signal_;
	sigslot::signal1<CommandInstance*> signalStateChanged_;
	int templateIndex_;
    
	bool autoCheck_;
	bool enabled_;
	bool checked_;
	CommandHotkey hotkey_;
};


class CommandTemplate{
public:
	CommandTemplate();
	CommandTemplate& add(const char* path, int& lastIndex, const char* label = 0);
	CommandTemplate& get(const char* path);
	CommandTemplate* find(const char* path);
	CommandTemplate* get(int index);

	typedef std::list<CommandTemplate> CommandTemplates;
	typedef CommandTemplates::iterator iterator;
	iterator begin(){ return children_.begin(); }
	iterator end(){ return children_.end(); }

	void serialize(Archive& ar);

	void purge(CommandTemplate& preservedOnes);
	void appendMissingFrom(CommandTemplate& templates);

	bool separate(bool separated){ separated_ = separated; }
	bool separated() const{ return separated_; }

	const char* name() const{ return name_.c_str(); }
	const char* label() const{ return label_.c_str(); }
	void setIndex(int index) { index_ = index; }
	int index() const{ return index_; }

	const CommandHotkey& hotkey() const{ return hotkey_; }
protected:
	CommandTemplates children_;
	CommandHotkey hotkey_;
	int index_;

	bool separated_;
	std::string name_;
	std::string label_;
};

class KDW_API CommandTemplateManager{
public:
	CommandTemplateManager();
	~CommandTemplateManager();
	int add(const char* text, const char* label = 0);
	CommandTemplate* get(int index);
	CommandTemplate* get(const char* text);

	static CommandTemplateManager& instance();
	sigslot::signal0& signalChanged() { return signalChanged_; }
	void serialize(Archive& ar);

	void setStateFile(const char* fileName);
protected:
	sigslot::signal0 signalChanged_;
	CommandTemplate root_;
	int lastTemplateIndex_;
	std::string stateFileName_;
};

class PopupMenuItem;
class KDW_API CommandManager : public sigslot::has_slots{
public:
	CommandManager();
	~CommandManager();
	CommandProxy get(const char* text, const char* label = 0){
		int templateIndex = CommandTemplateManager::instance().add(text, label);
		add(templateIndex);
		return CommandProxy(this, templateIndex);
	}
    
	void updateHotkeys();
	void setHotkeyContext(HotkeyContext* hotkeyContext);
	void onTemplatesChanged();
	void generateMenu(PopupMenuItem& item, int parentIndex);
	CommandInstance* commandInstanceByIndex(int templateIndex);

	void onHotkeyPressed(sKey key, bool& filtered);
	void onHotkeyReleased(sKey key, bool& filtered);
	void onStickyLocked();
	bool toggling() const{ return toggling_; }
	void delayToggle(){
		delayToggle_ = true;
		toggling_ = false;
	}
	sigslot::signal0& signalDelete(){ return signalDelete_; }
	bool invokeCommandInstance(CommandInstance* instance);
	bool stickyLocked() const{ return stickyLocked_; }
protected:
	void add(int templateIndex);
	void invokeCommand(int templateIndex);

	sigslot::signal0 signalDelete_;
	std::string stateFileName_;
	HotkeyContext* hotkeyContext_;
	CommandHotkey lastKey_;
	bool stickyLocked_;
	bool toggling_;
	bool delayToggle_;

	typedef std::vector<ShareHandle<CommandInstance> > CommandInstances;
	CommandInstances commandInstances_;
};

struct CommandManagerRef : public sigslot::has_slots{
public:
	CommandManagerRef(CommandManager* commands = 0);
	CommandManagerRef& operator=(CommandManager* commands);
	void connect(CommandManager* commands);
	void disconnect();

	operator CommandManager*() const{ return commandManager_; }
	CommandManager* operator->() const{ return commandManager_; }
	CommandManager& operator*() const{ return *commandManager_; }
protected:
	CommandManager* commandManager_;
};

}

#endif
