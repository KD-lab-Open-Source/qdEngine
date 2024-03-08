#include "StdAfx.h"
#include "kdw/CommandManager.h"
#include "kdw/Serialization.h"
#include "kdw/PopupMenu.h"

#include "FileUtils/FileUtils.h"
#include "Serialization/XPrmArchive.h"

namespace kdw{

CommandProxy::CommandProxy(CommandManager* commandManager, int templateIndex)
: commandManager_(commandManager)
, templateIndex_(templateIndex)
{
}

void CommandProxy::setSignal(CommandSignal* signal)
{
	CommandInstance* command = commandManager_->commandInstanceByIndex(templateIndex_);
	command->setSignal(signal);
}

CommandProxy& CommandProxy::setAutoCheck(bool autoCheck)
{
	CommandInstance* command = commandManager_->commandInstanceByIndex(templateIndex_);
	command->setAutoCheck(autoCheck);
	return *this;
}

bool CommandProxy::autoCheck() const
{
	CommandInstance* command = commandManager_->commandInstanceByIndex(templateIndex_);
	return command->autoCheck();
}

CommandProxy& CommandProxy::check(bool checked)
{
	CommandInstance* command = commandManager_->commandInstanceByIndex(templateIndex_);
	command->check(checked);
	return *this;
}

bool CommandProxy::checked() const
{
	CommandInstance* command = commandManager_->commandInstanceByIndex(templateIndex_);
	return command->checked();
}

CommandProxy& CommandProxy::enable(bool enabled)
{
	CommandInstance* command = commandManager_->commandInstanceByIndex(templateIndex_);
	command->enable(enabled);
	return *this;
}

bool CommandProxy::enabled() const
{
	CommandInstance* command = commandManager_->commandInstanceByIndex(templateIndex_);
	return command->enabled();
}

// ---------------------------------------------------------------------------

CommandHotkey::CommandHotkey()
: sticky_(false)
{
}

void CommandHotkey::serialize(Archive& ar)
{
	ar.serialize(key_, "key", "Хоткей");
	ar.serialize(sticky_, "sticky", "Липкий");
}


CommandTemplate::CommandTemplate()
: index_(-1)
, separated_(false)
{

}

CommandTemplate& CommandTemplate::add(const char* path, int& lastIndex, const char* label)
{
	const char* end = path;
	while(*end && *end != '.')
		++end;
	std::string name(path, end);
	CommandTemplates::iterator it;
	for(it = children_.begin(); it != children_.end(); ++it){
		CommandTemplate& command = *it;
		if(name == command.name()){
			if(*end == '.')
				return command.add(end + 1, lastIndex, label);
			else
				return command;
		}
	}

	children_.push_back(CommandTemplate());
	CommandTemplate& result = children_.back();
	result.name_ = std::string(path, end);
	result.index_ = ++lastIndex;
	if(*end == '.'){
		result.label_ = std::string(path, end);
		return result.add(end + 1, lastIndex, label);
	}
	else{
		result.label_ = label ? label : std::string(path, end);
		return result;
	}
}

CommandTemplate* CommandTemplate::find(const char* path)
{
	if(strcmp(path, ".") == 0)
		return this;
	const char* end = path;
	while(*end && *end != '.')
		++end;
	std::string name(path, end);
	CommandTemplates::iterator it;
	for(it = children_.begin(); it != children_.end(); ++it){
		CommandTemplate& command = *it;
		if(name == command.name()){
			if(*end == '.')
				return &command.get(end + 1);
			else
				return &command;
		}
	}
	return 0;
}

CommandTemplate& CommandTemplate::get(const char* path)
{
	CommandTemplate* result = find(path);
	ASSERT(result && "Комманда не найдена!");
	return *result;
}

CommandTemplate* CommandTemplate::get(int index)
{
	iterator it;
	FOR_EACH(children_, it){
		CommandTemplate& command = *it;
		if(command.index() == index)
			return &command;
		if(!command.children_.empty()){
			if(CommandTemplate* result = command.get(index))
				return result;
		}
	}
	return 0;
}

void CommandTemplate::serialize(Archive& ar)
{
	if(ar.isInput() || name_ != ""){
		ar.serialize(name_, "name", "&!");
		ar.serialize(label_, "label", "^");
		ar.serialize(separated_, "separate", "Отделять");
		if(ar.isEdit())
			hotkey_.serialize(ar);
		else
			ar.serialize(hotkey_, "hotkey", "Хоткей");
	}
	ar.serialize(children_, "children", "^!Элементы");
	if(ar.isEdit())
		ar.serialize(index_, "index", 0);
}

void CommandTemplate::purge(CommandTemplate& original)
{
	CommandTemplates::iterator it;
	for(it = children_.begin(); it != children_.end();){
		const char* name = it->name();
		if(CommandTemplate* templ = original.find(name)){
			it->setIndex(templ->index());
			it->purge(*templ);
		}
		else{
			it = children_.erase(it);
			continue;
		}
		++it;
	}
}

void CommandTemplate::appendMissingFrom(CommandTemplate& original)
{
	CommandTemplates::iterator it;
	for(it = original.children_.begin(); it != original.children_.end();){
		const char* name = it->name();
		if(CommandTemplate* templ = find(name)){
			templ->appendMissingFrom(*it);
		}
		else{
			children_.push_back(*it);
		}
		++it;
	}
}


// ---------------------------------------------------------------------------

CommandInstance::CommandInstance(int templateIndex)
: templateIndex_(templateIndex)
, checked_(false)
, enabled_(true)
, autoCheck_(false)
, signal_(0)
{

}

void CommandInstance::setSignal(CommandSignal* signal)
{
	signal_ = signal;
}

CommandTemplate& CommandInstance::getTemplate()
{
	CommandTemplate* result = CommandTemplateManager::instance().get(templateIndex_);
	ASSERT(result);
	return *result;
}

void CommandInstance::check(bool checked)
{
	checked_ = checked;
	signalStateChanged_.emit(this);
}

void CommandInstance::enable(bool enabled)
{
	enabled_ = enabled;
	signalStateChanged_.emit(this);
}

// ---------------------------------------------------------------------------
CommandTemplateManager::CommandTemplateManager()
: lastTemplateIndex_(0)
{
}

CommandTemplateManager::~CommandTemplateManager()
{
	if(!stateFileName_.empty()){
		XPrmOArchive oa(stateFileName_.c_str());
		oa.serialize(*this, "commandTemplates", 0);
	}
}

void CommandTemplateManager::setStateFile(const char* fileName)
{
	if(::isFileExists(fileName)){
		XPrmIArchive ia(fileName);
		ia.serialize(*this, "commandTemplates", 0);
	}
	stateFileName_ = fileName;
}

CommandTemplateManager& CommandTemplateManager::instance()
{
	static CommandTemplateManager commandTemplateManager;
	return commandTemplateManager;
}

int CommandTemplateManager::add(const char* text, const char* label)
{
	CommandTemplate& command = root_.add(text, lastTemplateIndex_, label);
	return command.index();
}

CommandTemplate* CommandTemplateManager::get(int index)
{
	return root_.get(index);
}

CommandTemplate* CommandTemplateManager::get(const char* text)
{
	return &root_.get(text);
}

void CommandTemplateManager::serialize(Archive& ar)
{
	if(ar.isEdit()){
		root_.serialize(ar);
	}
	else{
		if(ar.isInput()){
			CommandTemplate rootCopy = root_;
			root_.serialize(ar);
			root_.purge(rootCopy);
			root_.appendMissingFrom(rootCopy);
		}
		else{
			root_.serialize(ar);
		}
	}
	if(ar.isInput())
		signalChanged_.emit();
}


// ---------------------------------------------------------------------------


CommandManager::CommandManager()
: hotkeyContext_(0)
, toggling_(false)
, stickyLocked_(false)
{
	CommandTemplateManager::instance().signalChanged().connect(this, &CommandManager::onTemplatesChanged);
}

CommandManager::~CommandManager()
{
	signalDelete_.emit();
}
void CommandManager::updateHotkeys()
{
	if(hotkeyContext_){
		CommandInstances::iterator it;
		FOR_EACH(commandInstances_, it){
			CommandInstance* instance = *it;
			CommandTemplate* templ = CommandTemplateManager::instance().get(instance->templateIndex());
			instance->setHotkey(templ->hotkey());
		}
	}
}

void CommandManager::onTemplatesChanged()
{
	updateHotkeys();
}

void CommandManager::add(int templateIndex)
{
	CommandInstances::iterator it;
	FOR_EACH(commandInstances_, it){
		CommandInstance* inst = *it;
		if(inst->templateIndex() == templateIndex)
			return;
	}
	commandInstances_.push_back(new CommandInstance(templateIndex));
}

CommandInstance* CommandManager::commandInstanceByIndex(int templateIndex)
{
	CommandInstances::iterator it;
	FOR_EACH(commandInstances_, it){
		CommandInstance* command = *it;
		if(command->templateIndex() == templateIndex){
			return command;
		}
	}
	return 0;
}

void CommandManager::onStickyLocked()
{
	if(lastKey_.sticky() &&
		(GetAsyncKeyState(lastKey_.key().key) >> 15) && 
		(lastKey_.key().ctrl ? (GetAsyncKeyState(VK_CONTROL) >> 15) : 1) &&
		(lastKey_.key().shift ? (GetAsyncKeyState(VK_SHIFT) >> 15) : 1) &&
		(lastKey_.key().menu ? (GetAsyncKeyState(VK_MENU) >> 15) : 1)
	  )
	{
   		stickyLocked_ = true;
		toggling_ = false;
	}
}


void CommandManager::invokeCommand(int templateIndex)
{
	CommandInstance* command = commandInstanceByIndex(templateIndex);
	ASSERT(command);
	if(command){
		ASSERT(command->signal());
		ShareHandle<CommandSignal> signal(command->signal());
		if(command->autoCheck()){
			command->check(!command->checked());
		}
		signal->invoke();
	}
}

void CommandManager::generateMenu(PopupMenuItem& menu, int parentIndex)
{
	CommandTemplate* parent = CommandTemplateManager::instance().get(parentIndex);
	ASSERT(parent);
	CommandTemplate::iterator it;
	FOR_EACH(*parent, it){
		CommandTemplate& command = *it;

		if(command.separated() && !menu.empty())
			menu.addSeparator();
		std::string label = command.label();
		if(command.hotkey().key() != sKey()){
			label += "\t";
			label += command.hotkey().key().toString(true);
		}
		PopupMenuItem1<int>& item = menu.add(label.c_str(), command.index());
		if(CommandInstance* commandInstance = commandInstanceByIndex(command.index())){
			item.check(commandInstance->checked());
			item.enable(commandInstance->enabled());
			if(commandInstance->signal())
				item.connect(this, &CommandManager::invokeCommand);
		}
		generateMenu(item, command.index());
	}
}

void CommandManager::setHotkeyContext(HotkeyContext* hotkeyContext)
{
	hotkeyContext_ = hotkeyContext;
	if(hotkeyContext_){
		hotkeyContext_->signalStickyLocked().connect(this, &CommandManager::onStickyLocked);
		hotkeyContext_->signalPressedAny().connect(this, &CommandManager::onHotkeyPressed);
		hotkeyContext_->signalReleasedAny().connect(this, &CommandManager::onHotkeyReleased);
		updateHotkeys();
	}
}

void CommandManager::onHotkeyPressed(sKey key, bool& filtered)
{
	CommandInstances::iterator it;
	CommandInstances instances = commandInstances_;
	FOR_EACH(instances, it){
		CommandInstance* instance = *it;
		xassert(instance);
		if(instance->hotkey().key() == key){
			lastKey_ = instance->hotkey();
			if(invokeCommandInstance(instance))
				filtered = true;
		}
	}
	toggling_ = false;
}

bool CommandManager::invokeCommandInstance(CommandInstance* instance)
{
	ShareHandle<CommandInstance> ref(instance);
	if(instance->autoCheck())
		instance->check(!instance->checked());
	ShareHandle<CommandSignal> signal = instance->signal();
	if(signal){
		signal->invoke();
		return true;
	}
	return false;
}

void CommandManager::onHotkeyReleased(sKey key, bool& filtered)
{
	if(key != lastKey_.key())
		return;

	if(!stickyLocked_ && !delayToggle_)
		toggling_ = true;
	if(stickyLocked_ || delayToggle_){

		CommandInstances::iterator it;
		CommandInstances instances = commandInstances_;
		FOR_EACH(instances, it){
			CommandInstance* instance = *it;
			xassert(instance);
			if(instance->hotkey().key() == key){
				if(invokeCommandInstance(instance))
					filtered = true;

			}
		}

		stickyLocked_ = false;
		delayToggle_ = false;
	}
}

// ---------------------------------------------------------------------------
CommandManagerRef::CommandManagerRef(CommandManager* commands)
{
	connect(commands);
}

CommandManagerRef& CommandManagerRef::operator=(CommandManager* commands)
{
	connect(commands);
	return *this;
}

void CommandManagerRef::connect(CommandManager* commands)
{
	disconnect_all();
	if(commands)
		commands->signalDelete().connect(this, &CommandManagerRef::disconnect);
	commandManager_ = commands;
}

void CommandManagerRef::disconnect()
{
	commandManager_ = 0;
}

}
