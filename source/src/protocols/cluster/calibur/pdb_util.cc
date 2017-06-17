// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file external/calibur/pdb_util.hh
/// @author SC Li & YK Ng (kalngyk@gmail.com)
/// @author Andy Watkins (amw579@stanford.edu)

#define LONGEST_CHAIN 4000

#include <iostream>
#include <fstream>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iomanip>

#include <map>
#include <vector>

#include <protocols/cluster/calibur/SimPDB.fwd.hh>
#include <protocols/cluster/calibur/pdb_util.hh>

#include <memory>

namespace protocols {
namespace cluster {
namespace calibur {

char aa[][4] = {"BCK","GLY","ALA","SER","CYS","VAL","THR","ILE",
"PRO","MET","ASP","ASN","LEU",
"LYS","GLU","GLN","ARG",
"HIS","PHE","TYR","TRP","CYX", "MSE"};

char slc[] = {'X','G','A','S','C','V','T','I',
'P','M','D','N','L','K','E','Q','R',
'H','F','Y','W','C', 'm'};



typedef std::vector< std::string > StringVec;
typedef std::shared_ptr< StringVec > StringVecOP;

//- = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = - = -

int toInt( std::string const & aString ) {
	static char st[20];
	int start = 0;
	for ( unsigned int i=0; i < aString.size(); i++ ) {
		if ( aString[i] != ' ' ) {
			st[start++] = aString[i];
		}
	}
	st[start] = '\0';
	int rev = atoi(st);
	return rev;
}


double toFloat( std::string const & aString ) {
	static char st[20];
	int start = 0;
	for ( unsigned int i=0; i < aString.size(); i++ ) {
		if ( aString[i] != ' ' ) {
			st[start++] = aString[i];
		}
	}
	st[start] = '\0';
	double rev = atof(st);
	return rev;
}


void center_residues( std::vector<double> & calpha_vector, int num_residue_ ) {
	double cx = 0;
	double cy = 0;
	double cz = 0;

	int i3 = 0;
	for ( int i=0; i < num_residue_; i++ ) {
		cx += calpha_vector[i3];
		cy += calpha_vector[i3+1];
		cz += calpha_vector[i3+2];
		i3 += 3;
	}

	cx /= num_residue_;
	cy /= num_residue_;
	cz /= num_residue_;
	i3 = 0;
	for ( int i=0; i < num_residue_; i++ ) {
		calpha_vector[i3]   -= cx;
		calpha_vector[i3+1] -= cy;
		calpha_vector[i3+2] -= cz;
		i3 += 3;
	}
}


INPUT_FILE_TYPE
filetype( std::string const & filename )
{
	char buf[400];
	std::ifstream input(filename);
	std::string line;
	if ( !input ) {
		std::cerr << "Can't open input file \"" << filename << "\"" << std::endl;
		exit(0);
	}

	input.getline(buf, 400);
	line = buf;
	if ( line.substr(0, 9)=="SEQUENCE:" ) {
		input.close();
		return SILENT_FILE;
	}

	input.seekg(0);
	input.getline(buf, 400);
	char *token = strtok(buf, " ");
	if ( token == NULL ) {
		input.close();
		return UNKNOWN;
	}
	char *name = new char[strlen(token)+1];
	strcpy(name, token);

	std::ifstream pdbfile(name); // check if name is a file
	if ( pdbfile ) {
		pdbfile.close();
		input.close();
		return PDB_LIST;
	}
	pdbfile.close();

	input.close();

	return UNKNOWN;
}


unsigned int num_lines_in_file(std::string const & filename) {
	char buf[400];
	std::ifstream input(filename);
	if ( !input ) {
		std::cerr << "Can't open input file \"" << filename << "\"" << std::endl;
		exit(0);
	}
	unsigned int count = 0;
	while ( !input.eof() )
			{
		input.getline(buf, 400);
		count++;
	}
	return count-1;
}

}
}
}
