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

/*
 * File:   e2command.cpp
 * Author: user
 *
 * Created on December 11, 2022, 8:21 PM
 */



#include "e2command.h"
#include "E2wxApp.h"
#include "apple2.h"
#include "memoryrandomaccess.h"
#include "memorychip.h"
#include "cassette.h"
#include "cassettein.h"
#include "cassetteout.h"
#include "card.h"
#include "slots.h"
#include "diskcontroller.h"
#include "languagecard.h"
#include "firmwarecard.h"
#include "standardout.h"
#include "standardin.h"
#include "clockcard.h"
#include "screenimage.h"

#include <wx/filedlg.h>
#include <wx/string.h>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>



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

static void trim(std::string& s) {
    wxString w{s};
    s = w.Trim(false).Trim(true);
}

static std::string filter_row(const std::string &row) {
    if (row.length() != 1) {
        return "";
    }
    return std::string(1, static_cast<char> (std::toupper(row[0])));
}



E2Command::E2Command() {
}

E2Command::~E2Command() {
}



void E2Command::parseLine(const std::string& line, MemoryRandomAccess& ram, Memory& rom, Slots& slts, int& revision, ScreenImage& gui, CassetteIn& cassetteIn, CassetteOut& cassetteOut, Apple2* apple2) {
    try {
        tryParseLine(line, ram, rom, slts, revision, gui, cassetteIn, cassetteOut, apple2);
    } catch (const ConfigException& err) {
        // TODO fix error handling
        std::cerr << err.msg.c_str() << std::endl;
    }
}



void E2Command::tryParseLine(const std::string& line, MemoryRandomAccess& ram, Memory& rom, Slots& slts, int& revision, ScreenImage& gui, CassetteIn& cassetteIn, CassetteOut& cassetteOut, Apple2* apple2) {
    std::istringstream tok(line);

    std::string cmd;
    tok >> cmd;
    if (cmd == "slot") {
        int slot;
        std::string sCardType;
        tok >> slot >> sCardType;

        double random_ones_rate(0.3); // WOZ spec v2.0: 30%
        if (sCardType=="disk" || sCardType=="disk13") {
            tok >> random_ones_rate;
        }

        insertCard(sCardType, slot, slts, gui, random_ones_rate);
    } else if (cmd == "motherboard") {
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
    } else if (cmd == "import") {
        std::string sm;
        tok >> sm;

        int slot(-1);
        if (sm == "slot") {
            tok >> slot;
        } else if (sm != "motherboard") {
            throw ConfigException("error at \"" + sm + "\"; expected \"slot #\" or \"motherboard\"");
        }

        std::string romtype;
        tok >> romtype;

        unsigned short base(0);
        tok >> std::hex >> base;

        std::string file;
        std::getline(tok, file);
        trim(file);
        std::ifstream *memfile = new std::ifstream(file.c_str(), std::ios::binary);
        if (!memfile->is_open()) {
            std::filesystem::path f = wxGetApp().GetResDir();
            f /= file;
            memfile = new std::ifstream(f, std::ios::binary);
            if (!memfile->is_open()) {
                throw ConfigException("cannot open file " + file);
            }
        }

        if (slot < 0) // motherboard
        {
            if (romtype == "rom") {
                rom.load(base, *memfile);
            } else {
                throw ConfigException("error at \"" + romtype + "\"; expected rom or ram");
            }
        } else {
            if (8 <= slot) {
                throw ConfigException("invalid slot number");
            }
            Card* card = slts.get(slot);
            if (romtype == "rom")
                card->loadRom(base, *memfile);
            else if (romtype == "rom7")
                card->loadSeventhRom(base, *memfile);
            else if (romtype == "rombank")
                card->loadBankRom(base, *memfile);
            else
                throw ConfigException("error at \"" + romtype + "\"; expected rom, rom7, or rombank");
        }
        memfile->close();
    } else if (cmd == "load" || cmd == "save" || cmd == "unload") {
        std::string slotk;
        tok >> slotk;
        if (slotk != "slot") {
            throw ConfigException("error at \"" + slotk + "\"; expected \"slot\"");
        }

        int slot(-1);
        tok >> slot;

        std::string drivek;
        tok >> drivek;
        if (drivek != "drive") {
            throw ConfigException("error at \"" + drivek + "\"; expected \"drive\"");
        }

        int drive(-1);
        tok >> drive;

        if (cmd == "load") {
            std::string fn_optional;
            std::getline(tok, fn_optional);
            trim(fn_optional);
            if (fn_optional.length() == 0) {
                gui.exitFullScreen();
                wxFileDialog dlg{nullptr, "Load floppy", "", "", "WOZ 2.0 disk images (*.woz)|*.woz", wxFD_OPEN|wxFD_FILE_MUST_EXIST};
                if (dlg.ShowModal() == wxID_OK) {
                    fn_optional = dlg.GetPath().c_str();
                }
            }
            if (fn_optional.length() > 0) {
                loadDisk(slts, slot, drive, fn_optional);
            }
        } else if (cmd == "unload") {
            unloadDisk(slts, slot, drive);
        } else if (cmd == "save") {
            saveDisk(slts, slot, drive);
        }
    } else if (cmd == "revision") {
        tok >> std::hex >> revision;
    } else if (cmd == "cassette") {
        std::string cas;
        tok >> cas;

        if (cas == "rewind") {
            cassetteIn.rewind();
        } else if (cas == "tone") {
            cassetteIn.tone();
        } else if (cas == "blank") {
            std::string fcas;
            std::getline(tok, fcas);
            trim(fcas);
            if (!fcas.empty()) {
                cassetteOut.blank(fcas);
            }
        } else if (cas == "load") {
            std::string fn_optional;
            std::getline(tok, fn_optional);
            trim(fn_optional);
            if (fn_optional.length() == 0) {
                gui.exitFullScreen();
                wxFileDialog dlg{nullptr, "Load cassette (audio)", "", "", "WAVE cassette images (*.wav)|*.wav", wxFD_OPEN|wxFD_FILE_MUST_EXIST};
                if (dlg.ShowModal() == wxID_OK) {
                    fn_optional = dlg.GetPath().c_str();
                }
            }
            if (fn_optional.length() > 0) {
                cassetteIn.load(fn_optional);
            }
        } else if (cas == "eject") {
            std::string eject;
            tok >> eject;
            if (eject == "in") {
                cassetteIn.eject();
            } else if (eject == "out") {
                cassetteOut.eject();
            } else {
                throw ConfigException("error: unknown cassette to eject: " + eject);
            }
        } else if (cas == "save") {
            cassetteOut.save();
        } else {
            throw ConfigException("error: unknown cassette command: " + cas);
        }
    } else if (cmd == "cpu") {
        std::string cpu;
        tok >> cpu;
        if (apple2 != NULL) {
            if (cpu == "epple2") {
                apple2->useEpple2Cpu();
            } else if (cpu == "visual6502") {
                apple2->useVisual6502Cpu();
            } else {
                throw ConfigException("invalid value for cpu command: " + cpu);
            }
        }
    } else {
        throw ConfigException("Invalid command: " + cmd);
    }

    if (apple2 != NULL) {
        apple2->useEpple2Cpu(); // set default CPU
    }
}



void E2Command::loadDisk(Slots& slts, int slot, int drive, const std::filesystem::path &media) {
    Card* card = slts.get(slot);
    if (!card->hasMedia()) {
        // TODO if file doesn't exist, name still gets displayed, and there's no error message
        // TODO error message
        // std::cerr << "Slot " << slot << " doesn't have a disk controller card" << std::endl;
        return;
    }

    if (drive < 1 || 2 < drive) {
        throw ConfigException("Invalid drive; must be 1 or 2");
    }

    card->loadMedia(drive - 1, media);
}

void E2Command::unloadDisk(Slots& slts, int slot, int drive) {
    Card* card = slts.get(slot);
    if (!card->hasMedia()) {
        // TODO do we even need an error here?
        // std::cerr << "Slot " << slot << " doesn't have a disk controller card" << std::endl;
        return;
    }

    if (drive < 1 || 2 < drive) {
        throw ConfigException("Invalid drive; must be 1 or 2");
    }

    card->unloadMedia(drive - 1);
}

void E2Command::saveDisk(Slots& slts, int slot, int drive) {
    Card* card = slts.get(slot);
    if (!card->hasMedia()) {
        // TODO do we even need an error here?
        // std::cerr << "Slot " << slot << " doesn't have a disk controller card" << std::endl;
        return;
    }

    if (drive < 1 || 2 < drive) {
        throw ConfigException("Invalid drive; must be 1 or 2");
    }

    card->saveMedia(drive - 1);
}



void E2Command::insertCard(const std::string& cardType, const int slot, Slots& slts, ScreenImage& gui, const double random_ones_rate) {
    if (slot < 0 || 8 <= slot) {
        throw ConfigException("Invalid slot number");
    }

    Card* card = nullptr;

    if (cardType == "language") {
        card = new LanguageCard(gui, slot);
    } else if (cardType == "firmware") {
        card = new FirmwareCard(gui, slot);
    } else if (cardType == "disk") {
        card = new DiskController(gui, slot, false, random_ones_rate);
    } else if (cardType == "disk13") {
        card = new DiskController(gui, slot, true, random_ones_rate);
    } else if (cardType == "clock") {
        card = new ClockCard();
    } else if (cardType == "stdout") {
        card = new StandardOut();
    } else if (cardType == "stdin") {
        card = new StandardIn();
    } else if (cardType == "empty") {
        card = 0;
    } else {
        throw ConfigException("Invalid card type: " + cardType);
    }

    if (card) {
        slts.set(slot, card);
    } else {
        slts.remove(slot);
    }
}
