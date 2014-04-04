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

#ifndef SILVET_EM_H
#define SILVET_EM_H

#include <vector>

class EM
{
public:
    EM();
    ~EM();

    void iterate(std::vector<double> column);
    void report();

private:
    typedef std::vector<double> V;
    typedef std::vector<std::vector<double> > Grid;

    V m_pitches;
    Grid m_sources;

    V m_estimate;
    V m_q;
    
    int m_notes;
    int m_bins;
    int m_instruments;

    int m_lowest;
    int m_highest;

    void normalise(V &column);
    void expectation(const V &column);
    void maximisation(const V &column);

    bool inRange(int instrument, int note);
};

#endif