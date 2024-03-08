#include "StdAfx.h"

#include "kdw/Application.h"
#include "kdw/Window.h"

#include "kdw/VBox.h"
#include "kdw/HBox.h"
#include "kdw/VSplitter.h"
#include "kdw/HSplitter.h"
#include "kdw/ScrolledWindow.h"
#include "kdw/Tabs.h"

#include "kdw/Toolbar.h"
#include "kdw/ImageStore.h"
#include "kdw/PropertyTree.h"

#include "kdw/Entry.h"
#include "kdw/ComboBox.h"
#include "kdw/Label.h"
#include "kdw/Button.h"
#include "kdw/ProgressBar.h"
#include "kdw/RadioButton.h"
#include "kdw/RadioButtonBox.h"
#include "kdw/CheckBox.h"
#include "kdw/Frame.h"
#include "kdw/Dialog.h"
#include "kdw/Serialization.h"
#include "Serialization/XPrmArchive.h"
#include "kdw/HotkeyDialog.h"
#include "kdw/Clipboard.h"
#include "kdw/PropertyEditor.h"

#include "kdw/LibraryEditor.h"
#include "kdw/LibraryEditorDialog.h"
#include "ParameterLibrary.h"
#include "kdw/CommandManager.h"

#include "XMath/XMathLib.h"

#include "TestData.h"

class IntButton : public kdw::Button{
public:
	IntButton(const char* text, int value)
	: kdw::Button(text, 0)
	, value_(value)
	{

	}

	void onPressed(){
		signalPressed_.emit(value_);
	}
	sigslot::signal1<int>& signalPressed() { return signalPressed_; }
protected:
	sigslot::signal1<int> signalPressed_;

	int value_;
};

class MainWindow : public kdw::Window{
public:
	MainWindow(kdw::Application& app);

	void onTextChanged();
	void onIntButtonPressed(int index);
	void onChangeTextButton();
	void onChangeCheckBox();
	void onEditButton();
	void pressShowModal();
protected:
	kdw::VBox* vbox_;
	kdw::Entry* edit_;
	kdw::Button* buttonChangeText_;
	kdw::ProgressBar* progressBar_;
	kdw::PropertyTree* propertyTree_;
	kdw::Dialog* dialog; 
	kdw::LibraryEditor* libraryEditor_;
};

TestData testData;

MainWindow::MainWindow(kdw::Application& app)
{
	setTitle("My test application");
	//setIconFromFile("exodus.ico");
	setBorder(0);
	setMinimizeable(true);
	setResizeable(true);

	setDefaultSize(Vect2i(800, 600));
	setDefaultPosition(kdw::POSITION_CENTER);

	signalClose().connect((Win32::MessageLoop*)&app, &Win32::MessageLoop::quit);

	kdw::TabPages* pages = new kdw::TabPages();
	add(pages); 
	{	
		kdw::VBox* vbox = new kdw::VBox;
		pages->add("Тест интерфейса", vbox);
		{
			kdw::CommandManager* commands = new kdw::CommandManager();
			kdw::Toolbar* toolbar = new kdw::Toolbar(commands);

			kdw::ImageStore* imageStore = new kdw::ImageStore(24, 24);
			imageStore->addFromFile("res\\FX_PAN_256.bmp", RGB(255, 0, 255));
			toolbar->setImageStore(imageStore);

			commands->get("main.test1", "Проверка");
			commands->get("main.test2", "Второй");
			commands->get("main.test3", "Третий");
			toolbar->addButton("main.test1", 0);
			toolbar->addButton("main.test2", 1);
			toolbar->addButton("main.test3", 2);
			vbox->add(toolbar);

			kdw::HSplitter* splitter_ = new kdw::HSplitter(0, 0);
			vbox->add(splitter_, true, true, true);
			{
				kdw::VSplitter* vsplitter = new kdw::VSplitter(0, 0);
				splitter_->add(vsplitter, 0.25f);
				{
					kdw::VBox* vbox = new kdw::VBox(4, 6);
					//vsplitter->add(vbox);
					{
						vbox->add(new kdw::Label("Группа 1:", true));

						kdw::ComboBox* comboBox = new kdw::ComboBox;
						comboBox->add("Вариант 1");
						comboBox->add("Вариант 2");
						comboBox->add("Вариант 3");
						vbox->add(comboBox);

						kdw::Button* button = new kdw::Button("В&ыход");
						button->signalPressed().connect((Win32::MessageLoop*)&app, &Win32::MessageLoop::quit);
						vbox->add(button, false, false, false);

						kdw::CheckBox* checkBox = new kdw::CheckBox("Text Check Box");
						checkBox->signalChanged().connect(this, &MainWindow::onChangeCheckBox);
						vbox->add(checkBox, false, false, false);
						
						kdw::RadioButtonBox* radioButtonBox = new kdw::RadioButtonBox("RadioBtnBox");
						radioButtonBox->addRadioButton("Radio 1");
						radioButtonBox->addRadioButton("Radio 2");
						radioButtonBox->addRadioButton("Radio 3");
						radioButtonBox->addRadioButton("Radio 4");
						vbox->add(radioButtonBox, false, false, false);

						progressBar_ = new kdw::ProgressBar();
						progressBar_->setPosition(0.5f);
						vbox->add(progressBar_, false, false, false);

						kdw::RadioButton* rBtn1 = new kdw::RadioButton(0, "RadioButton 1");
						vbox->add(rBtn1, false, false, false);
						//delete rBtn1;

						buttonChangeText_ = new kdw::Button("Добавить кнопок");
						buttonChangeText_->signalPressed().connect(this, &MainWindow::onChangeTextButton);
						vbox->add(buttonChangeText_, false, true, true);

						
						button = new kdw::Button("Редактировать");
						button->signalPressed().connect(this, MainWindow::onEditButton);
						vbox->add(button, false, false, false);

						kdw::Label* label = new kdw::Label("Текстовое &поле:", true);
						vbox->add(label, false);

						kdw::RadioButton* rBtn2 = new kdw::RadioButton(rBtn1, "RadioButton 2");
						vbox->add(rBtn2, false, false, false);
						kdw::RadioButton* rBtn3 = new kdw::RadioButton(rBtn2, "RadioButton 3");
						vbox->add(rBtn3, false, false, false);

						edit_ = new kdw::Entry("Text", true);
						edit_->signalChanged().connect(this, &MainWindow::onTextChanged);

						kdw::HBox* hbox = new kdw::HBox(4, 0);
						{
							button = new kdw::Button("Кнопка 3");
							button->setSensitive(false);
							button = new kdw::Button("Кнопочка 4");
						}

					}
					kdw::Frame* frame_ = new kdw::Frame();
					frame_->add(vbox);
					vsplitter->add(frame_, 0.5f);

					kdw::ScrolledWindow* scrolledWindow = new kdw::ScrolledWindow(6);
					vsplitter->add(scrolledWindow);
					{
						vbox_ = new kdw::VBox(4, 0);
						scrolledWindow->add(vbox_);
						scrolledWindow->setPolicy(kdw::SCROLL_NEVER, kdw::SCROLL_AUTOMATIC);
						{
							kdw::Label* label = new kdw::Label("Сдесь будет много кнопочек");
							label->setAlignment(kdw::ALIGN_CENTER, kdw::ALIGN_MIDDLE);
							vbox_->add(label, false, true, true);
						}
					}
				}
				propertyTree_ = new kdw::PropertyTree;
				propertyTree_->setSelectFocused(true);
				propertyTree_->setHideUntranslated(true);
				propertyTree_->setCompact(true);
				splitter_->add(propertyTree_, 0.75);

				kdw::Box* box = new kdw::VBox;
				box->add(new kdw::Button("First button", 6), false);
				box->add(new kdw::Button("Last button", 6), true);
				splitter_->add(box);
			}

			vbox->add(new kdw::Label("Статусная строка", false, 2), true, false, false);
		}

		libraryEditor_ = new kdw::LibraryEditor();
		pages->add("Редактор библиотек", libraryEditor_);
	}

	propertyTree_->attach(Serializer(testData));
}

void MainWindow::onTextChanged()
{
	buttonChangeText_->setText(edit_->text());
}
	
void MainWindow::pressShowModal()
{
	//kdw::HotkeyDialog dlg(this);
	//dlg.showModal();

	TestBase testBase;
	testBase.base_member = "Pasted from clipboard";
	testBase.base_float = 98765.f;
	kdw::Clipboard clip(this);
	clip.copy(Serializer(testBase, "", ""));
}

void MainWindow::onIntButtonPressed(int index)
{
	//XBuffer buf;
	//buf < "Нажата кнопка с индексом: " <= index;
	//const char* message = buf;
	const char* buf = "Button pressed";

	buttonChangeText_->setText(buf);
}

void MainWindow::onChangeCheckBox()
{
	progressBar_->setPosition(rand()*1.f/RAND_MAX);
}

void MainWindow::onChangeTextButton()
{
	//buttonChangeText_->setText("Текст изменился на очень длинную строку");
	static int counter = 0;
	//XBuffer buf;
	//buf < "Кнопка ся! " <= counter;
	const char* buf = "Кнопка ся";

	IntButton* newButton = new IntButton(buf, counter++);
	newButton->signalPressed().connect(this, &MainWindow::onIntButtonPressed);
	vbox_->add(newButton, false, true, true);

	newButton = new IntButton(buf, counter++);
	newButton->signalPressed().connect(this, &MainWindow::onIntButtonPressed);
	vbox_->add(newButton, false, true, true);

	newButton = new IntButton(buf, counter++);
	newButton->signalPressed().connect(this, &MainWindow::onIntButtonPressed);
	vbox_->add(newButton, false, false, false);


	vbox_->showAll();
}

TestBase base;
TestDerivedA a;
TestDerivedB b;

void MainWindow::onEditButton()
{
	kdw::LibraryEditorDialog dialog(this);

	XPrmIArchive ia;
	if(ia.open("LibraryEditorState")){
		ia.setFilter(kdw::SERIALIZE_STATE);
		dialog.serialize(ia);
	}
	
	loadAllLibraries();
	if(dialog.showModal() == kdw::RESPONSE_OK)
		saveAllLibraries();

	XPrmOArchive oa("LibraryEditorState");
	oa.setFilter(kdw::SERIALIZE_STATE);
	dialog.serialize(oa);

	return;

	TestData test;
	kdw::edit(Serializer(test), "testState", kdw::IMMEDIATE_UPDATE | kdw::ONLY_TRANSLATED, this);
	return;

	base.base_float = 3.0f;
	a.base_float = 3.0f;
	b.base_float = 3.0f;
	b.base_flag = true;

	base.base_member = "BASE";
	a.base_member = "A";
	b.base_member = "B";

	Serializers sers;
	sers.push_back(Serializer(base));
	sers.push_back(Serializer(a));
	sers.push_back(Serializer(b));
    
	propertyTree_->attach(sers);
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	kdw::Application app(hInstance);

	ShareHandle<MainWindow> window = new MainWindow(app);
	window->showAll();

	int result = app.run();
	return result;
}


bool GetAllTextureNames(const char* filename, std::vector<std::string>& result)
{
	xassert(0 && "Unimplimented");
	return false;
}
