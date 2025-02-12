#include <wx/wx.h>

#include <Generator.h>
#include <filesystem>
#include <wx/clipbrd.h>

#include <SQLiteCpp/SQLiteCpp.h>

enum
{
    ID_CB_LOWERCASE,
    ID_CB_UPPERCASE,
    ID_CB_NUMBERS,
    ID_CB_SYMBOLS,
    ID_BTN_GENERATE,
    ID_BTN_COPY,
    ID_BTN_HASH
};

class MyFrame : public wxFrame
{
public:
    MyFrame()
        :
        wxFrame(nullptr, wxID_ANY, "Password Generator GUI", wxDefaultPosition, wxSize(800, 600)),
        passwordGenerator(policy),
        db("PasswordGenDB", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
    {
        if (sodium_init() < 0)
        {
            std::cerr << "Failed to initialize libsodium" << std::endl;
            throw std::runtime_error("Failed to initialize libsodium");
        }

        // setup database
        db.exec("CREATE TABLE IF NOT EXISTS passwords (hash TEXT)");

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

        // mess with fonts here
        wxWindow::SetFont(wxFont(12, wxFONTFAMILY_ROMAN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

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

        // horizontal sizer holding the slider and label
        wxBoxSizer* sliderSizer = new wxBoxSizer(wxHORIZONTAL);

        passwordLenText = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100, 25));
        passwordLenText->SetForegroundColour(lightText);
        passwordLenText->SetBackgroundColour(darkControlBg);
        sliderSizer->Add(passwordLenText, 0, wxALL, 5);
        passwordLenSlider = new wxSlider(panel, wxID_ANY, 10, 10, 1000, wxDefaultPosition, wxSize(350, 15), wxSL_HORIZONTAL);
        passwordLenSlider->SetBackgroundColour(darkControlBg);
        sliderSizer->Add(passwordLenSlider, 0, wxALL, 5);

        mainSizer->Add(sliderSizer, 0, wxALIGN_CENTER);

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
        generateBtn->SetBackgroundColour(darkControlBg);
        generateBtn->SetForegroundColour(lightText);
        generateSizer->Add(generateBtn, 0, wxALL, 5);

        // Read-only text field for the generated password. passwordText is a member
        passwordText = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(250, 25),
            wxTE_READONLY);
        passwordText->SetBackgroundColour(darkControlBg);
        passwordText->SetForegroundColour(lightText);
        generateSizer->Add(passwordText, 0, wxALL, 5);

        // Copy button.
        wxButton* copyBtn = new wxButton(panel, ID_BTN_COPY, "Copy");
        copyBtn->SetBackgroundColour(darkControlBg);
        copyBtn->SetForegroundColour(lightText);
        generateSizer->Add(copyBtn, 0, wxALL, 5);

        mainSizer->Add(generateSizer, 0, wxALIGN_CENTER);

        // --- Row for cryptography and database ---
        wxBoxSizer* extraSizer = new wxBoxSizer(wxHORIZONTAL);

        // Hash password button
        wxButton* hashPasswordButton = new wxButton(panel, ID_BTN_HASH, "Hash Password");
        hashPasswordButton->SetBackgroundColour(darkControlBg);
        hashPasswordButton->SetForegroundColour(lightText);
        extraSizer->Add(hashPasswordButton, 0, wxALL, 5);

        // Read-only text field for the hash. passwordText is a member
        hashText = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(600, 25),
            wxTE_READONLY);
        hashText->SetBackgroundColour(darkControlBg);
        hashText->SetForegroundColour(lightText);
        extraSizer->Add(hashText, 0, wxALL, 5);

        // Button to save hashes to the database
        wxButton* saveBtn = new wxButton(panel, wxID_ANY, "Save Hash to Database");
        saveBtn->SetBackgroundColour(darkControlBg);
        saveBtn->SetForegroundColour(lightText);
        extraSizer->Add(saveBtn, 0, wxALL, 5);


        mainSizer->Add(extraSizer, 0, wxALIGN_CENTER);


        // Set the main sizer for the panel.
        panel->SetSizer(mainSizer);


        // bind slider
        passwordLenSlider->Bind(wxEVT_SCROLL_THUMBTRACK, [&](wxScrollEvent& event)
        {
            policy.passwordLength = passwordLenSlider->GetValue();
            passwordGenerator.SetPolicy(policy);
            passwordLenText->ChangeValue(std::to_string(policy.passwordLength));
        });

        passwordLenText->Bind(wxEVT_TEXT, [&](wxCommandEvent& event)
        {
            passwordLenSlider->SetValue(passwordLenSlider->GetValue());
            policy.passwordLength = passwordLenSlider->GetValue();
            passwordGenerator.SetPolicy(policy);
        });

        // Bind text control event if needed.
        excludedCharsTextCtrl->Bind(wxEVT_TEXT, [&](wxCommandEvent& event)
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

        // Hash password button event.
        hashPasswordButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event)
        {
            if (passwordText->GetValue().empty())
            {
                std::cerr << "Password text is empty!" << std::endl;
                wxLogError("Password text is empty!");
                return;
            }
            try
            {
                // keep memory safety. Store plain-text password for as little time as possible
                std::vector<std::string> password = { (std::string)passwordText->GetValue() };
                auto hash = passwordGenerator.HashPasswordsSafeAsync(std::move(password));
                hash.wait();
                passwordText->ChangeValue(wxEmptyString);
                hashText->ChangeValue(hash.get().front());
            }
            catch (const std::exception& ex)
            {
                wxLogError("Exception during password hashing: %s", ex.what());
                std::cerr << "Exception during password hashing: " << ex.what() << std::endl;
            }
            catch (...)
            {
                wxLogError("Unknown exception during password hashing.");
                std::cerr << "Unknown exception during password hashing." << std::endl;
            }
        });

        saveBtn->Bind(wxEVT_BUTTON, [&](wxCommandEvent& event)
        {
            try
            {
                SQLite::Statement insertQuery(db, "INSERT INTO passwords (hash) VALUES (?)");

                insertQuery.bind(1, (std::string)hashText->GetValue());
                insertQuery.exec();
                insertQuery.reset();

                // for debugging
                SQLite::Statement selectQuery(db, "SELECT * FROM passwords");
                while (selectQuery.executeStep())
                {
                    std::cout << selectQuery.getColumn(0) << std::endl;
                }
                selectQuery.reset();
            }
            catch (const std::exception& ex)
            {
                std::cerr << "Exception during database insertion: " << ex.what() << std::endl;
                wxLogError("Exception during database insertion: %s", ex.what());
            }
            catch (...)
            {
                std::cerr << "Unknown exception during database insertion." << std::endl;
                wxLogError("Unknown exception during database insertion.");
            }
        });
    }

private:
    Generator::PasswordPolicy policy;
    Generator::PasswordGenerator passwordGenerator;

    // If this wasn't a member, it would result in a segfault for who knows what reason when you change it.
    wxTextCtrl* passwordText = nullptr;
    wxTextCtrl* hashText = nullptr;

    wxSlider* passwordLenSlider;
    // label to display the current slider value. editable
    wxTextCtrl* passwordLenText;

    SQLite::Database db;


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