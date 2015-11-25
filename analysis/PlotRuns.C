//C++ Headers
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <map>
#include <stdio.h>
#include <string>
#include <sstream>

//ROOT Headers
#include "TString.h"
#include "TMath.h"
#include "TH1F.h"
#include "THStack.h"
#include "TLegend.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TArrow.h"
#include "TText.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TMultiGraph.h"
#include "TRolke.h"
#include "TLatex.h"

////ROOT MACROS Headers
int MAX_MOVIES = 2200;

using namespace std;
class movie {

	int title_pos, year_pos, rated_pos, released_pos, genre_pos, runtime_pos, imdb_votes_pos, imdb_rating_pos, imdb_id_pos;
	string title, year, rated, released, genre, runtime, imdb_votes, imdb_rating, imdb_id;
	int yearAsInt, runtimeAsInt, votesAsInt;
	float ratingAsFloat;

	string inpt_string;
	public:
		void set_inpt(string);
		void set_title();
		void set_year();
		void set_rated();
		void set_released();
		void set_runtime();
		void set_genre();
		void set_imdb_rating();
		void set_imdb_votes();
		void set_imdb_id();
};


void movie::set_inpt(string inpt)
{
	inpt_string = inpt;
}

void movie::set_title()
{
	int begin = inpt_string.find("Title");
	int t_begin = begin+8;
	int t_end = inpt_string.find("\"",t_begin+1);
	title = inpt_string.substr(t_begin,t_end-t_begin);
	cout<<"Title: "<<title<<"\n";
	title_pos = t_end;
}

void movie::set_year()
{
	int begin = inpt_string.find("Year",title_pos+1);
	int t_begin = begin+7;
	int t_end = inpt_string.find("\"",t_begin+1);
	year = inpt_string.substr(t_begin,t_end-t_begin);
	cout<<"Year: "<<year<<"\n";
	year_pos = t_end;
	
	yearAsInt = atoi(year.c_str()); 
}

void movie::set_rated()
{
	int begin = inpt_string.find("Rated",year_pos+1);
	int t_begin = begin+8;
	int t_end = inpt_string.find("\"",t_begin+1);
	rated = inpt_string.substr(t_begin,t_end-t_begin);
	cout<<"Rated: "<<rated<<"\n";
	rated_pos = t_end;
}

void movie::set_released()
{
	int begin = inpt_string.find("Released",rated_pos+1);
	int t_begin = begin+11;
	int t_end = inpt_string.find("\"",t_begin+1);
	released = inpt_string.substr(t_begin,t_end-t_begin);
	cout<<"Released: "<<released<<"\n";
	released_pos = t_end;
}

void movie::set_runtime()
{
	int begin = inpt_string.find("Runtime",released_pos+1);
	int t_begin = begin+10;
	int t_end = inpt_string.find("\"",t_begin+1);
	runtime = inpt_string.substr(t_begin,t_end-t_begin);
	cout<<"Runtime: "<<runtime<<"\n";
	runtime_pos = t_end;
	
	runtimeAsInt = atoi(runtime.c_str()); 
}

void movie::set_genre()
{
	int begin = inpt_string.find("Genre",runtime_pos+1);
	int t_begin = begin+8;
	int t_end = inpt_string.find("\"",t_begin+1);
	genre = inpt_string.substr(t_begin,t_end-t_begin);
	cout<<"Genre: "<<genre<<"\n";
	genre_pos = t_end;
}

void movie::set_imdb_rating()
{
	int begin = inpt_string.find("imdbRating",genre_pos+1);
	int t_begin = begin+13;
	int t_end = inpt_string.find("\"",t_begin+1);
	imdb_rating = inpt_string.substr(t_begin,t_end-t_begin);
	cout<<"IMDB Rating: "<<imdb_rating<<"\n";
	imdb_rating_pos = t_end;

	ratingAsFloat = atof(imdb_rating.c_str());
	cout<<"Rating: "<<ratingAsFloat<<"\n";
}
void movie::set_imdb_votes()
{
	int begin = inpt_string.find("imdbVotes",imdb_rating_pos+1);
	int t_begin = begin+12;
	int t_end = inpt_string.find("\"",t_begin+1);
	imdb_votes = inpt_string.substr(t_begin,t_end-t_begin);
	cout<<"IMDB Votes: "<<imdb_votes<<"\n";
	imdb_votes_pos = t_end;

	imdb_votes.replace(imdb_votes.find(","),1,"");
	if(imdb_votes.find(",")!=string::npos) imdb_votes.replace(imdb_votes.find(","),1,"");

	votesAsInt = atoi(imdb_votes.c_str());
}

void movie::set_imdb_id()
{
	int begin = inpt_string.find("imdbID",imdb_votes_pos+1);
	int t_begin = begin+9;
	int t_end = inpt_string.find("\"",t_begin+1);
	imdb_id = inpt_string.substr(t_begin,t_end-t_begin);
	cout<<"IMDB ID: "<<imdb_id<<"\n";
	imdb_id_pos = t_end;
}

void CountEvents()
{
	cout<<"Starting...\n";
	string line;
	string saveline;
	ifstream input_file1;
	input_file1.open("../0.json");
	
	int linecount = 0;
	
	if(input_file1.is_open())
	{
		while(getline(input_file1,line))
		{
			saveline = line;
		}
		input_file1.close();

	}

	vector<string> raw_movies;
	int x = 0, x_prev = 1;
	for(int i = 0; i < MAX_MOVIES; i++)
	{	
		x = saveline.find("{", x_prev+1);
		cout<<x_prev<<" | "<<x<<"\n";
	
		raw_movies.push_back(saveline.substr(x_prev,(x-x_prev)));
		x_prev = x;
	}
	
	movie *mov_all;
	
	TFile *f = new TFile("test.root","recreate");
	TTree *t1 = new TTree("t1","Test Tree 1");
	t1->Branch("movie",&mov_all);

	int votes[2200];
	float rating[2200];
	
	for(int i = 0; i < MAX_MOVIES; i++)
	{
		mov_all = new movie();

		cout<<"\n"<<raw_movies[i]<<"\n";
		mov_all->set_inpt(raw_movies[i]);
		mov_all->set_title();
		mov_all->set_year();
		mov_all->set_rated();
		mov_all->set_released();
		mov_all->set_runtime();
		mov_all->set_genre();	
		mov_all->set_imdb_rating();
		mov_all->set_imdb_votes();
		mov_all->set_imdb_id();

		t1->Fill();
		
		delete mov_all;
	}	
	
	t1->Write();
}


