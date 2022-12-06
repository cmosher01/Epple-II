/*
    epple2
    Copyright (C) 2008 by Christopher A. Mosher <cmosher01@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY, without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "configep2.h"

#include "E2wxApp.h"

#include "apple2.h"
#include "memory.h"
#include "memoryrandomaccess.h"
#include "slots.h"
#include "diskcontroller.h"
#include "languagecard.h"
#include "firmwarecard.h"
#include "standardout.h"
#include "standardin.h"
#include "clockcard.h"
#include "cassettein.h"
#include "cassetteout.h"
#include "tinyfiledialogs.h"

#include <wx/config.h>
#include <wx/string.h>

#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cctype>



#define K 1024u

static std::uint16_t memory_block_size(const std::string &block_size) {
    if (block_size == "4K") {
        return 4u*K;
    }
    if (block_size == "16K") {
        return 16u*K;
    }
    throw ConfigException("invalid RAM strapping block size (must be 4K or 16K)");
}



unsigned char Config::disk_mask(0);

Config::Config(const std::string& file_path):
    file_path(file_path)
{
}


Config::~Config()
{
}

static void strip_comment(std::string& str)
{
    const size_t comment = str.find('#');
    if (comment < std::string::npos)
    {
        str.erase(comment);
    }
}

static void trim(std::string& str)
{
    {
        const size_t p = str.find_first_not_of(" \t");
        if (p < std::string::npos)
        {
            str.erase(0,p);
        }
    }
    {
        const size_t p = str.find_last_not_of(" \t");
        if (p+1 < std::string::npos)
        {
            str.erase(p+1);
        }
    }
}

void Config::parse(MemoryRandomAccess& ram, Memory& rom, Slots& slts, int& revision, ScreenImage& gui, CassetteIn& cassetteIn, CassetteOut& cassetteOut, Apple2* apple2)
{
    std::ifstream* pConfig;

    std::string path(this->file_path);

    if (!path.empty())
    {
        pConfig = new std::ifstream(path.c_str());
        if (!pConfig->is_open())
        {
            // TODO use filename only and look in standard resources
            std::stringstream ss;
            ss << "Cannot open config file " << this->file_path.c_str();
            throw std::runtime_error(ss.str());
        }
    }



    if (path.empty())
    {
        // TODO config file location, how to be backwardly compatible?
        wxString user_config;
        if (!wxConfigBase::Get()->Read(wxT("/ActivePreferences/name"), &user_config)) {
            // TODO what to do when no config?
            user_config = wxT("epple2");
        }

        std::filesystem::path f = wxGetApp().GetConfigDir();
        f /= user_config.t_str();
        f += ".conf";
        path = f.string();
        std::cout << "looking for config file: " << path << std::endl;
        pConfig = new std::ifstream(path.c_str());
        if (!pConfig->is_open()) {
            f = wxGetApp().GetResDir();
            f /= user_config.t_str();
            f += ".conf";
            path = f.string();
            std::cout << "looking for config file: " << path << std::endl;
            pConfig = new std::ifstream(path.c_str());
            if (!pConfig->is_open()) {
                path.clear();
            }
        }
    }




    if (path.empty())
    {
        std::cout << "standard config file location: " ETCDIR "/epple2/epple2.conf" << std::endl;

        /*
            On Windows, the default directory will be
            C:\Program Files\Epple2 if they start the
            program from the Start Menu; therefore
            etc/epple2/epple2.conf would be
            C:\Program Files\epple2\etc\epple2\epple2.conf
            On Linux... the current directory could be
            anything, so this probably won't find it (unless
            the current directory is /).
        */
        path = "etc/epple2/epple2.conf";
        pConfig = new std::ifstream(path.c_str());
        if (!pConfig->is_open())
            path.clear();
    }
    if (path.empty())
    {
        /*
            This is primarily for Linux. If configured for
            a PREFIX of "/usr/local", then this would be
            /usr/local/etc/epple2/epple2.conf
        */
        path = ETCDIR "/epple2/epple2.conf";
        pConfig = new std::ifstream(path.c_str());
        if (!pConfig->is_open())
            path.clear();
    }
    if (path.empty())
    {
        /*
            Try a likely linux location
        */
        path = "/etc/epple2/epple2.conf";
        pConfig = new std::ifstream(path.c_str());
        if (!pConfig->is_open())
            path.clear();
    }
    if (path.empty())
    {
        /*
            Try another likely linux location
        */
        path = "/etc/epple2.conf";
        pConfig = new std::ifstream(path.c_str());
        if (!pConfig->is_open())
            path.clear();
    }
    if (path.empty())
    {
        /*
            Last effort to find it.
        */
        path = "epple2.conf";
        pConfig = new std::ifstream(path.c_str());
        if (!pConfig->is_open())
            path.clear();
    }
    if (path.empty())
    {
        std::cerr << "Cannot find config file. Running without any RAM, ROM, or cards." << std::endl;
        return;
    }

    std::cout << "Reading configuration from file: " << path << std::endl;

    std::string line;
    std::getline(*pConfig,line);
    while (!pConfig->eof())
    {
        strip_comment(line);
        trim(line);
        if (!line.empty())
        {
            parseLine(line,ram,rom,slts,revision,gui,cassetteIn,cassetteOut,apple2);
        }
        std::getline(*pConfig,line);
    }
    pConfig->close();
    delete pConfig;

    // TODO: make sure there is no more than ONE stdin and/or ONE stdout card
}
void Config::parseLine(const std::string& line, MemoryRandomAccess& ram, Memory& rom, Slots& slts, int& revision, ScreenImage& gui, CassetteIn& cassetteIn, CassetteOut& cassetteOut, Apple2* apple2)
{
    try
    {
        tryParseLine(line,ram,rom,slts,revision,gui,cassetteIn,cassetteOut,apple2);
    }
    catch (const ConfigException& err)
    {
        std::cerr << err.msg.c_str() << std::endl;
    }
}

static std::string filter_row(const std::string &row) {
    if (row.length() != 1) {
        return "";
    }
    return std::string(1, static_cast<char>(std::toupper(row[0])));
}

void Config::tryParseLine(const std::string& line, MemoryRandomAccess& ram, Memory& rom, Slots& slts, int& revision, ScreenImage& gui, CassetteIn& cassetteIn, CassetteOut& cassetteOut, Apple2* apple2)
{
    std::istringstream tok(line);

    std::string cmd;
    tok >> cmd;
    if (cmd == "slot")
    {
        int slot;
        std::string sCardType;
        tok >> slot >> sCardType;

        insertCard(sCardType,slot,slts,gui,tok);
    }
    else if (cmd == "motherboard") {
        std::string op;
        tok >> op;
        if (op == "ram") {
            /* ram ROW BIT0 [BIT1 [... [BIT7]]]
             * ram e -
             * ram d 4096 MK4096 4K
             * ram c 16K 4116 MK4116 MM5290 16K 16K 16K 16K
             */
            std::string row;
            tok >> row;
            row = filter_row(row);
            if (row != "C" && row != "D" && row != "E") {
                throw ConfigException("expected row to be C, D, or E");
            }

            std::string chip_model;
            tok >> chip_model;
            for (std::uint_fast8_t bit = 0u; bit < 8u; ++bit) {
                ram.insert_chip(row, MemoryChip::instance(chip_model), bit);

                std::string chip_model_optional;
                tok >> chip_model_optional;
                if (chip_model_optional.length()) {
                    chip_model = chip_model_optional;
                }
            }
        } else if (op == "strap") {
            /* strap ROM K start-addr
             * strap e 4K 5000
             * strap d 4K 4000
             * strap c 16K 0000
             */
            std::string row;
            tok >> row;
            row = filter_row(row);
            if (row != "C" && row != "D" && row != "E") {
                throw ConfigException("expected row to be C, D, or E");
            }

            std::string block_size;
            tok >> block_size;
            std::uint16_t siz = memory_block_size(block_size);
            unsigned short base(0);
            tok >> std::hex >> base;
            // TODO validate siz/base combination
            ram.strap_to(row, base, siz);
        } else {
            throw ConfigException("error at \"motherboard\"; expected \"ram\" or \"strap\"");
        }
    }
    else if (cmd == "import")
    {
        std::string sm;
        tok >> sm;

        int slot(-1);
        if (sm == "slot")
        {
            tok >> slot;
        }
        else if (sm != "motherboard")
        {
            throw ConfigException("error at \""+sm+"\"; expected \"slot #\" or \"motherboard\"");
        }

        std::string romtype;
        tok >> romtype;

        unsigned short base(0);
        tok >> std::hex >> base;

        std::string file;
        std::getline(tok,file);
        trim(file);
        std::ifstream *memfile = new std::ifstream(file.c_str(),std::ios::binary);
        if (!memfile->is_open())
        {
            std::filesystem::path f = wxGetApp().GetResDir();
            f /= file;
            memfile = new std::ifstream(f,std::ios::binary);
            if (!memfile->is_open())
            {
                throw ConfigException("cannot open file "+file);
            }
        }

        if (slot < 0) // motherboard
        {
            if (romtype == "rom")
            {
                rom.load(base,*memfile);
            }
            else
            {
                throw ConfigException("error at \""+romtype+"\"; expected rom or ram");
            }
        }
        else
        {
            if (8 <= slot)
            {
                throw ConfigException("invalid slot number");
            }
            Card* card = slts.get(slot);
            if (romtype == "rom")
                card->loadRom(base,*memfile);
            else if (romtype == "rom7")
                card->loadSeventhRom(base,*memfile);
            else if (romtype == "rombank")
                card->loadBankRom(base,*memfile);
            else
                throw ConfigException("error at \""+romtype+"\"; expected rom, rom7, or rombank");
        }
        memfile->close();
    }
    else if (cmd == "load" || cmd == "save" || cmd == "unload")
    {
        std::string slotk;
        tok >> slotk;
        if (slotk != "slot")
        {
            throw ConfigException("error at \""+slotk+"\"; expected \"slot\"");
        }

        int slot(-1);
        tok >> slot;

        std::string drivek;
        tok >> drivek;
        if (drivek != "drive")
        {
            throw ConfigException("error at \""+drivek+"\"; expected \"drive\"");
        }

        int drive(-1);
        tok >> drive;

        if (cmd == "load")
        {
            std::string fn_optional;
            std::getline(tok,fn_optional);
            trim(fn_optional);
            if (fn_optional.length() == 0) {
                gui.exitFullScreen();
                char const *ft[1] = { "*.woz" };
                char const *fn = tinyfd_openFileDialog("Load floppy", "", 1, ft, "WOZ 2.0 disk images", 0);
                if (fn) {
                    fn_optional = std::string(fn);
                }
            }
            if (fn_optional.length() > 0) {
                // TODO check if file exists, if not then check resources
                loadDisk(slts,slot,drive,fn_optional);
            }
        }
        else if (cmd == "unload")
        {
            unloadDisk(slts,slot,drive);
        }
        else if (cmd == "save")
        {
            saveDisk(slts,slot,drive);
        }
    }
    else if (cmd == "revision")
    {
        tok >> std::hex >> revision;
    }
    else if (cmd == "cassette")
    {
        std::string cas;
        tok >> cas;

        if (cas == "rewind")
        {
            cassetteIn.rewind();
        }
        else if (cas == "tone")
        {
            cassetteIn.tone();
        }
        else if (cas == "blank")
        {
            std::string fcas;
            std::getline(tok,fcas);
            trim(fcas);
            if (!fcas.empty()) {
                cassetteOut.blank(fcas);
            }
        }
        else if (cas == "load")
        {
            std::string fn_optional;
            std::getline(tok,fn_optional);
            trim(fn_optional);
            if (fn_optional.length() == 0) {
                gui.exitFullScreen();
                char const *ft[1] = { "*.wav" };
                char const *fn = tinyfd_openFileDialog("Load cassette audio", "", 1, ft, "WAVE cassette images", 0);
                if (fn) {
                    fn_optional = std::string(fn);
                }
            }
            if (fn_optional.length() > 0) {
                cassetteIn.load(fn_optional);
            }
        }
        else if (cas == "eject")
        {
            std::string eject;
            tok >> eject;
            if (eject == "in") {
                cassetteIn.eject();
            } else if (eject == "out") {
                cassetteOut.eject();
            } else {
                throw ConfigException("error: unknown cassette to eject: "+eject);
            }
        }
        else if (cas == "save")
        {
            cassetteOut.save();
        }
        else
        {
            throw ConfigException("error: unknown cassette command: "+cas);
        }
    }
    else if (cmd == "cpu")
    {
        std::string cpu;
        tok >> cpu;
        if (apple2 != NULL) {
            if (cpu == "epple2") {
                apple2->useEpple2Cpu();
            } else if (cpu == "visual6502") {
                apple2->useVisual6502Cpu();
            } else {
                throw ConfigException("invalid value for cpu command: "+cpu);
            }
        }
    }
    else
    {
        throw ConfigException("Invalid command: "+cmd);
    }

    if (apple2 != NULL) {
        apple2->useEpple2Cpu(); // set default CPU
    }
}

void Config::loadDisk(Slots& slts, int slot, int drive, const std::string& fnib)
{
    if (drive < 1 || 2 < drive)
    {
        throw ConfigException("Invalid drive; must be 1 or 2");
    }

    // TODO if file doesn't exist, name still gets displayed, and there's no error message
    Card* card = slts.get(slot);
    if (!(disk_mask & (1 << slot)))
    {
        std::cerr << "Slot " << slot << " doesn't have a disk controller card" << std::endl;
        return;
    }

    DiskController* controller = (DiskController*)card;
    controller->loadDisk(drive-1,fnib);
}

void Config::unloadDisk(Slots& slts, int slot, int drive)
{
    if (drive < 1 || 2 < drive)
    {
        throw ConfigException("Invalid drive; must be 1 or 2");
    }

    Card* card = slts.get(slot);
    if (!(disk_mask & (1 << slot)))
    {
        std::cerr << "Slot " << slot << " doesn't have a disk controller card" << std::endl;
        return;
    }

    DiskController* controller = (DiskController*)card;
    controller->unloadDisk(drive-1);
}

void Config::saveDisk(Slots& slts, int slot, int drive)
{
    if (drive < 1 || 2 < drive)
    {
        throw ConfigException("Invalid drive; must be 1 or 2");
    }
    slts.get(slot)->save(drive-1);
}

void Config::insertCard(const std::string& cardType, int slot, Slots& slts, ScreenImage& gui, std::istringstream& tok)
{
    if (slot < 0 || 8 <= slot)
    {
        throw ConfigException("Invalid slot number");
    }

    Card* card;

    disk_mask &= ~(1 << slot);

    if (cardType == "language")
    {
        card = new LanguageCard(gui,slot);
    }
    else if (cardType == "firmware")
    {
        card = new FirmwareCard(gui,slot);
    }
    else if (cardType == "disk") // 16-sector LSS ROM
    {
        double random_ones_rate(0.3); // WOZ spec v2.0: 30%
        tok >> random_ones_rate;
        std::cerr << "MC3470: rate of 1 bits during random bit generation: " << random_ones_rate << std::endl;
        card = new DiskController(gui,slot,false,random_ones_rate);
        disk_mask |= (1 << slot);
    }
    else if (cardType == "disk13") // 13-sector LSS ROM
    {
        double random_ones_rate(0.3); // WOZ spec v2.0: 30%
        tok >> random_ones_rate;
        std::cerr << "MC3470: rate of 1 bits during random bit generation: " << random_ones_rate << std::endl;
        card = new DiskController(gui,slot,true,random_ones_rate);
        disk_mask |= (1 << slot);
    }
    else if (cardType == "clock")
    {
        card = new ClockCard();
    }
    else if (cardType == "stdout")
    {
        card = new StandardOut();
    }
    else if (cardType == "stdin")
    {
        card = new StandardIn();
    }
    else if (cardType == "empty")
    {
        card = 0;
    }
    else
    {
        throw ConfigException("Invalid card type: "+cardType);
    }

    if (card)
        slts.set(slot,card);
    else
        slts.remove(slot);
}
