#include <wx/wx.h>

enum
{
    ID_HELLO = 1
};

class MyFrame : public wxFrame
{
public:
    MyFrame()
        :
        wxFrame(nullptr, wxID_ANY, "Hello World")
    {
        wxMenu* menuFile = new wxMenu;
        menuFile->Append(ID_HELLO, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
        menuFile->AppendSeparator();
        menuFile->Append(wxID_EXIT);

        wxMenu* menuHelp = new wxMenu;
        menuHelp->Append(wxID_ABOUT);

        wxMenuBar* menuBar = new wxMenuBar;
        menuBar->Append(menuFile, "&File");
        menuBar->Append(menuHelp, "&Help");

        wxFrameBase::SetMenuBar(menuBar);

        wxFrameBase::CreateStatusBar();
        wxFrameBase::SetStatusText("Welcome to wxWidgets!");

        Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_HELLO);
        Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
        Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
    }

private:
    void OnHello(wxCommandEvent& event)
    {
        wxLogMessage("Hello world from wxWidgets!");
    }
    void OnExit(wxCommandEvent& event)
    {
        Close(true);
    }
    void OnAbout(wxCommandEvent& event)
    {
        wxMessageBox("This is a wxWidgets application", "About Hello World", wxOK | wxICON_INFORMATION);
    }
};

class MyApp : public wxApp
{
public:
    bool OnInit() override
    {
        // no memory leak, wxwidgets has ownership
        auto* frame = new MyFrame();
        frame->Show();
        return true;
    }
};

// defines the equivalent of main() for current platform
wxIMPLEMENT_APP(MyApp);