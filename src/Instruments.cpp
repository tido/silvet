/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
  Silvet

  A Vamp plugin for note transcription.
  Centre for Digital Music, Queen Mary University of London.
    
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.  See the file
  COPYING included with this distribution for more information.
*/

#include "Instruments.h"

#include "data/include/templates.h"

#include <iostream>

const int InstrumentPack::templateNoteCount = SILVET_TEMPLATE_NOTE_COUNT;
const int InstrumentPack::templateHeight = SILVET_TEMPLATE_HEIGHT;
const int InstrumentPack::templateMaxShift = SILVET_TEMPLATE_MAX_SHIFT;
const int InstrumentPack::templateSize = SILVET_TEMPLATE_SIZE;

using std::string;
using std::vector;
using std::cerr;
using std::endl;

const char *simpleInstruments[] = {
    "Guitar", "guitar",
    "Violin", "violin",
    "Cello", "cello",
    "Horn", "horn",
    "Flute", "flute",
    "Oboe", "oboe",
    "Clarinet", "clarinet",
    "Tenor Sax", "tenorsax",
    "Bassoon", "bassoon",
};

static InstrumentPack::Templates
templatesFor(string name)
{
    for (int i = 0; i < SILVET_TEMPLATE_COUNT; ++i) {

	if (string(silvet_templates[i].name) == name) {

	    silvet_template_t *t = &silvet_templates[i];

	    InstrumentPack::Templates rt;
	    rt.lowestNote = t->lowest;
	    rt.highestNote = t->highest;
	    rt.data = vector<vector<float> >
		(SILVET_TEMPLATE_NOTE_COUNT,
		 vector<float>(SILVET_TEMPLATE_SIZE, 0.f));

	    for (int j = 0; j < SILVET_TEMPLATE_NOTE_COUNT; ++j) {
		for (int k = 0; k < SILVET_TEMPLATE_SIZE; ++k) {
		    rt.data[j][k] = t->data[j][k];
		}
	    }

	    return rt;
	}
    }

    return InstrumentPack::Templates();
}

static bool 
isOK(InstrumentPack &d)
{
    if (d.name == "") {
	cerr << "ERROR: Silvet::InstrumentPack: Empty name in instrument definition" << endl;
	return false;
    }
    if (d.templates.empty()) {
	cerr << "ERROR: Silvet::InstrumentPack: Instrument definition \"" << d.name << "\" contains no templates!" << endl;
	return false;
    }
    for (int i = 0; i < int(d.templates.size()); ++i) {
	if (d.templates[i].data.empty()) {
	    cerr << "ERROR: Silvet::InstrumentPack: Instrument definition \"" << d.name << "\" contains one or more empty templates!" << endl;
	    return false;
	}
    }
    return true;
}

vector<InstrumentPack> 
InstrumentPack::listInstrumentPacks()
{
    vector<InstrumentPack> ii;

    vector<Templates> allTemplates;
    //!!! is piano-maps-SptkBGCl the same as one of piano1, piano2, piano3?
    allTemplates.push_back(templatesFor("piano1"));

    vector<Templates> pianoTemplates;
    pianoTemplates.push_back(templatesFor("piano1"));
    pianoTemplates.push_back(templatesFor("piano2"));
    pianoTemplates.push_back(templatesFor("piano3"));
    InstrumentPack piano(silvet_templates_lowest_note,
			 silvet_templates_highest_note,
			 "Piano",
			 pianoTemplates);
    if (isOK(piano)) {
	ii.push_back(piano);
    }

    for (int i = 0;
	 i < int(sizeof(simpleInstruments)/sizeof(simpleInstruments[0]));
	 i += 2) {
	vector<Templates> tt;
	Templates t = templatesFor(simpleInstruments[i+1]);
	tt.push_back(t);
	allTemplates.push_back(t);
	InstrumentPack instr(t.lowestNote,
			     t.highestNote,
			     simpleInstruments[i],
			     tt);
	if (isOK(instr)) {
	    ii.push_back(instr);
	}
    }

    InstrumentPack all(silvet_templates_lowest_note,
		       silvet_templates_highest_note,
		       "Multiple or unknown instruments",
		       allTemplates);
    if (isOK(all)) {
	ii.insert(ii.begin(), all);
    }

    return ii;
}

