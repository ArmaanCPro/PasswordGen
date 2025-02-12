#include <wx/wx.h>

#include <Generator.h>

enum
{
    ID_HELLO = 1,
    ID_CB_LOWERCASE,
    ID_CB_UPPERCASE,
    ID_CB_NUMBERS,
    ID_CB_SYMBOLS
};

class MyFrame : public wxFrame
{
public:
    MyFrame()
        :
        wxFrame(nullptr, wxID_ANY, "Hello World", wxDefaultPosition, wxSize(800, 600)),
        passwordGenerator(policy)
    {
        // Create a panel to hold the controls.
        wxPanel* panel = new wxPanel(this, wxID_ANY);

        // Create a vertical box sizer for overall layout.
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Create a horizontal sizer to hold the label and text field side by side.
        wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);

        // Create the label. wxStaticText is non-editable.
        wxStaticText* label = new wxStaticText(panel, wxID_ANY, "Excluded characters:");

        // Create the text field.
        wxTextCtrl* excludedCharsTextCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(100, 25));

        // Add the label and text control to the horizontal sizer with some borders.
        hSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 5);
        hSizer->Add(excludedCharsTextCtrl, 0, wxALL, 5);

        // Add the horizontal sizer to the main sizer.
        mainSizer->Add(hSizer, 0, wxALIGN_CENTER);

        // --- Checkboxes for password policy ---
        // Create checkboxes for each boolean option.
        wxCheckBox* cbLowercase = new wxCheckBox(panel, ID_CB_LOWERCASE, "Require Lowercase");
        wxCheckBox* cbUppercase = new wxCheckBox(panel, ID_CB_UPPERCASE, "Require Uppercase");
        wxCheckBox* cbDigits    = new wxCheckBox(panel, ID_CB_NUMBERS, "Require Numbers");
        wxCheckBox* cbSymbols   = new wxCheckBox(panel, ID_CB_SYMBOLS, "Require Special Characters");

        // Create a vertical sizer for checkboxes.
        wxBoxSizer* cbSizer = new wxBoxSizer(wxVERTICAL);
        cbSizer->Add(cbLowercase, 0, wxALL, 5);
        cbSizer->Add(cbUppercase, 0, wxALL, 5);
        cbSizer->Add(cbDigits,    0, wxALL, 5);
        cbSizer->Add(cbSymbols,   0, wxALL, 5);

        // Add the checkbox sizer to the main sizer.
        mainSizer->Add(cbSizer, 0, wxALIGN_CENTER);


        // Set the main sizer for the panel.
        panel->SetSizer(mainSizer);

        // Setting up the menu.
        wxMenu* menuFile = new wxMenu;
        menuFile->Append(ID_HELLO, "&Hello...\tCtrl-H", "Help string shown in status bar for this menu item");
        menuFile->AppendSeparator();
        menuFile->Append(wxID_EXIT);

        wxMenu* menuHelp = new wxMenu;
        menuHelp->Append(wxID_ABOUT);

        wxMenuBar* menuBar = new wxMenuBar;
        menuBar->Append(menuFile, "&File");
        menuBar->Append(menuHelp, "&Help");
        SetMenuBar(menuBar);

        CreateStatusBar();
        SetStatusText("Welcome to wxWidgets!");

        // Bind events.
        Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_HELLO);
        Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
        Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);

        // Bind text control event if needed.
        Bind(wxEVT_TEXT, [&](wxCommandEvent& event)
        {
            policy.excludedCharacters = event.GetString();
            passwordGenerator.SetPolicy(policy);
        });

        // Bind checkbox events to update policy.
        cbLowercase->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& event)
        {
            policy.requireLowercase = event.IsChecked();
            passwordGenerator.SetPolicy(policy);
        });
        cbUppercase->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& event)
        {
            policy.requireUppercase = event.IsChecked();
            passwordGenerator.SetPolicy(policy);
        });
        cbDigits->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& event)
        {
            policy.requireNumbers = event.IsChecked();
            passwordGenerator.SetPolicy(policy);
        });
        cbSymbols->Bind(wxEVT_CHECKBOX, [&](wxCommandEvent& event)
        {
            policy.requireSymbols = event.IsChecked();
            passwordGenerator.SetPolicy(policy);
        });

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

private:
    Generator::PasswordGenerator passwordGenerator;
    Generator::PasswordPolicy policy;
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