#include <wx/wx.h>

#include <Generator.h>
#include <filesystem>
#include <wx/clipbrd.h>

enum
{
    ID_HELLO = 1,
    ID_CB_LOWERCASE,
    ID_CB_UPPERCASE,
    ID_CB_NUMBERS,
    ID_CB_SYMBOLS,
    ID_BTN_GENERATE,
    ID_BTN_COPY
};

class MyFrame : public wxFrame
{
public:
    MyFrame()
        :
        wxFrame(nullptr, wxID_ANY, "Password Generator GUI", wxDefaultPosition, wxSize(800, 600)),
        passwordGenerator(policy)
    {
        if (sodium_init() < 0)
        {
            std::cerr << "Failed to initialize libsodium" << std::endl;
            throw std::runtime_error("Failed to initialize libsodium");
        }

        // app icon shenanigans
        {
            std::filesystem::path iconPath = std::filesystem::absolute("password-icon.ico");
            wxIcon icon;
            icon.LoadFile(iconPath.string(), wxBITMAP_TYPE_ICO);

            // Check if the file exists
            if (!std::filesystem::exists(iconPath) || !icon.IsOk()) {
                wxMessageBox("Icon file not found: " + iconPath.string(), "Error", wxOK | wxICON_ERROR);
                return;
            }

            // Set the window icon
            SetIcon(icon);
        }

        // Define dark mode colors.
        wxColor darkBg(30, 30, 30);           // Dark background (almost black)
        wxColor darkControlBg(50, 50, 50);    // A slightly lighter tone for input controls
        wxColor lightText(220, 220, 220);     // Light text (almost white)

        // Light mode colors
        //wxColor lightControlBg(240, 240, 240);
        //wxColor darkText(50, 50, 50);
        //wxColor LightBg(250, 250, 250);

        // Set the frame's background color.
        wxWindowBase::SetBackgroundColour(darkBg);


        // Create a panel to hold the controls.
        wxPanel* panel = new wxPanel(this, wxID_ANY);
        panel->SetBackgroundColour(darkBg);

        // Create a vertical box sizer for overall layout.
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

        // Create a horizontal sizer to hold the label and text field side by side.
        wxBoxSizer* hSizer = new wxBoxSizer(wxHORIZONTAL);

        // Create the label. wxStaticText is non-editable.
        wxStaticText* label = new wxStaticText(panel, wxID_ANY, "Excluded characters:");
        label->SetForegroundColour(lightText);

        // Create the text field.
        wxTextCtrl* excludedCharsTextCtrl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100, 25));
        excludedCharsTextCtrl->SetBackgroundColour(darkControlBg);
        excludedCharsTextCtrl->SetForegroundColour(lightText);

        // Add the label and text control to the horizontal sizer with some borders.
        hSizer->Add(label, 0, wxALL | wxALIGN_CENTER, 5);
        hSizer->Add(excludedCharsTextCtrl, 0, wxALL, 5);

        // Add the horizontal sizer to the main sizer.
        mainSizer->Add(hSizer, 0, wxALIGN_CENTER);

        // --- Checkboxes for password policy ---
        // Create checkboxes for each boolean option.
        wxCheckBox* cbLowercase = new wxCheckBox(panel, ID_CB_LOWERCASE, "Require Lowercase");
        cbLowercase->SetValue(true);
        cbLowercase->SetForegroundColour(lightText);
        wxCheckBox* cbUppercase = new wxCheckBox(panel, ID_CB_UPPERCASE, "Require Uppercase");
        cbUppercase->SetValue(true);
        cbUppercase->SetForegroundColour(lightText);
        wxCheckBox* cbDigits    = new wxCheckBox(panel, ID_CB_NUMBERS, "Require Numbers");
        cbDigits->SetValue(true);
        cbDigits->SetForegroundColour(lightText);
        wxCheckBox* cbSymbols   = new wxCheckBox(panel, ID_CB_SYMBOLS, "Require Special Characters");
        cbSymbols->SetValue(true);
        cbSymbols->SetForegroundColour(lightText);

        // Create a vertical sizer for checkboxes.
        wxBoxSizer* cbSizer = new wxBoxSizer(wxVERTICAL);
        cbSizer->Add(cbLowercase, 0, wxALL, 5);
        cbSizer->Add(cbUppercase, 0, wxALL, 5);
        cbSizer->Add(cbDigits,    0, wxALL, 5);
        cbSizer->Add(cbSymbols,   0, wxALL, 5);

        // Add the checkbox sizer to the main sizer.
        mainSizer->Add(cbSizer, 0, wxALIGN_CENTER);


        // --- Row for generating password and copying ---
        wxBoxSizer* generateSizer = new wxBoxSizer(wxHORIZONTAL);

        // Generate password button
        wxButton* generateBtn = new wxButton(panel, ID_BTN_GENERATE, "Generate Password");
        generateSizer->Add(generateBtn, 0, wxALL, 5);

        // Read-only text field for the generated password. passwordText is a member
        passwordText = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250, 25),
            wxTE_READONLY);
        passwordText->SetBackgroundColour(darkControlBg);
        passwordText->SetForegroundColour(lightText);
        generateSizer->Add(passwordText, 0, wxALL, 5);

        // Copy button.
        wxButton* copyBtn = new wxButton(panel, ID_BTN_COPY, "Copy");
        generateSizer->Add(copyBtn, 0, wxALL, 5);

        mainSizer->Add(generateSizer, 0, wxALIGN_CENTER);



        // Set the main sizer for the panel.
        panel->SetSizer(mainSizer);


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

        // Generate password button event.
        generateBtn->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event)
        {
            try
            {
                // Call generator's method to produce a new password.
                auto newPassword = passwordGenerator.GenerateAdvancedPasswordsAsync(1);
                passwordText->ChangeValue(newPassword.get().front());
            } catch (const std::exception& ex) {
                wxLogError("Exception during password generation: %s", ex.what());
                std::cerr << "Exception during password generation: " << ex.what() << std::endl;
            } catch (...) {
                wxLogError("Unknown exception during password generation.");
                std::cerr << "Unknown exception during password generation." << std::endl;
            }
        });

        // Copy button event.
        copyBtn->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event)
        {
            if (!passwordText) {
                return;
            }

            wxString password = passwordText->GetValue();
            if (CopyToClipboard(password))
            {
                std::cout << "Password copied to clipboard!" << std::endl;
            }
            else
            {
                std::cerr << "Failed to copy to clipboard" << std::endl;
            }

            // Flush the clipboard to ensure the data stays after the application closes. May cause memory leak...
            wxTheClipboard->Flush();

            // Close the clipboard
            wxTheClipboard->Close();
        });

    }

private:
    Generator::PasswordPolicy policy;
    Generator::PasswordGenerator passwordGenerator;

    // If this wasn't a member, it would result in a segfault for who knows what reason when you change it.
    wxTextCtrl* passwordText;

private:
    static bool CopyToClipboard(const wxString& text) {
        // Guard against empty text
        if (text.empty())
        {
            return false;
        }

        // Ensure single clipboard access
        if (wxTheClipboard->IsOpened())
        {
            return false;
        }

        bool success = false;
        try {
            if (wxTheClipboard->Open())
            {
                // no data leak, the memory is handled by the clipboard
                auto* data = new wxTextDataObject(text);
                success = wxTheClipboard->SetData(data);
                wxTheClipboard->Close();
            }
        } catch (...) {
            if (wxTheClipboard->IsOpened())
            {
                wxTheClipboard->Close();
            }
            return false;
        }

        return success;
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