#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <array>
#include <random>
#include <datetimeapi.h> 
#include <time.h>
#include <memory>
#include <cmath>

using namespace std;
using namespace cv;


CvFont font;



void initFont()
{
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 3, 8);
}

void scrivi (CvArr* arr,const string& p) 
{
	cvPutText(arr, p.c_str(), cvPoint(100, 100), &font, cvScalar(0,0,255)); // B G R
};



int rndrange(int min, int max)
{
	return min + (rand() % (max-min+1));
}

void onchangeTrackbar(void* x) {
	

}


/** @function Dilation */
void Dilation(int dilation_elem, Mat& src, Mat& dilation_dst, int dilation_size = 1, int it = 1)
{
	int dilation_type;
	if (dilation_elem == 0) { dilation_type = MORPH_RECT; }
	else if (dilation_elem == 1) { dilation_type = MORPH_CROSS; }
	else if (dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
	else if (dilation_elem == 3)
	{
		dilate(src, dilation_dst, Mat(), Point(-1, -1), it);
		return;
	}

	Mat element = getStructuringElement(dilation_type,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));

	/// Apply the dilation operation
	dilate(src, dilation_dst, element, Point(-1, -1), it);
}


/**  @function Erosion  */
void Erosion(int erosion_elem, Mat& src, Mat& erosion_dst, int erosion_size = 0, int it=1)
{
	int erosion_type;
	if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
	else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
	else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
	else if (erosion_elem == 3)
	{
		erode(src, erosion_dst, Mat(), Point(-1, -1), it);
		return;
	}

	Mat element = getStructuringElement(erosion_type,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	/// Apply the erosion operation
	erode(src, erosion_dst, element, Point(-1, -1), it);
}


class Persona
{
public:
	int posx = 0;
	int posy = 0;
	int height = 0;
	int width = 0;
	
	vector< Rect > track;
	int id = 0;
	bool bool_attivo = true;
	
	time_t tempoinizio{ 0 };
	time_t tempofine{ 0 };
	time_t tmpcreato{ 0 };
	bool pcontata = false;
	int disattivata = 0;
	int presenza = 0;
	bool grande = false;
	int area_media = 0;
	bool bool_piccolo = false;
	Scalar color { 0 };


	Persona()
	{
		tempoinizio = time(nullptr);
		tempofine = time(nullptr);
		tmpcreato = time(nullptr);
		color = { (double)rndrange(1, 255), (double)rndrange(1, 255),(double)rndrange(1, 255) };
	}

	Persona(int _id)
	:Persona()
	{
		id = _id;
	}

 
	Vec2f VecDirection()
	{
		if (track.size() > 2)
		{
			size_t nframes = 60;
			size_t grand = std::max(track.size(), nframes);
			//if (track.size()) >= nframes )
			float sommax = 0;
			float sommay = 0;
			vector< Vec2f> centro;
			for (size_t x = track.size() - grand;
						x != track.size();
						++x) 
			{
				centro.push_back(Vec2f(track[x].x + (track[x].width / 2), track[x].y + (track[x].height / 2)));
			}

			for (size_t i = 0; i != centro.size() - 1; ++i )
			{
				float sommax=0;
				sommax += centro[i+1][0] - centro[i][0];
				float sommay=0;
				sommay += centro[i+1][1] - centro[i][1];
			}
			float xvel = 0;
			float yvel = 0;
			xvel = sommax / (grand - 1);
			yvel = sommay / (grand - 1);

			return { xvel, yvel };
		}
		return { 0, 0 };
	}


	float AreaMedia()
	{
		float media = 0;
		//se ci sono dei quadrati
		if (track.size())
		{
			//////////////////////////////////////////////////////
			size_t nframes = 20;
			size_t grand = std::max(track.size(), nframes);
			//////////////////////////////////////////////////////
			for (size_t i = track.size() - grand;
			i != track.size();
				++i)
			{
				media += track[i].width * track[i].height;
			}
			//////////////////////////////////////////////////////
			media /= grand;
		}
		//salva risultato
		if (grande)
		{
			float area_media = media;
		}
		//ritorna risultato
		return media;
	}


	Vec2i CalcolaCentro()
	{
		Vec2i point;
		point[0] = posx + (width / 2);
		point[1]  = posy + (height / 2);
		return point;
	}

	void TrackingPersona(const Vec2i& ptmin,const Vec2i& ptmax)
	{
		posx = ptmin[0];
		posy = ptmin[1];
		height = ptmax[1] - ptmin[1];
		width = ptmax[0] - ptmin[0];
		track.push_back({ posx, posy, width, height });
		tempofine = time(nullptr);
	}

	double CalcolaTempo() const
	{
		return difftime(tempofine , tempoinizio);
	}	
};

int main()
{
	initFont();

	
	namedWindow("camera", CV_WINDOW_AUTOSIZE); 
	namedWindow("camera2", CV_WINDOW_AUTOSIZE);
	namedWindow("configurazione", CV_WINDOW_NORMAL);

	int iSliderValue1 = 10;
	createTrackbar("dilate", "configurazione", &iSliderValue1, 100); //10 
	int iSliderValue2 = 3;
	createTrackbar("erode", "configurazione", &iSliderValue2, 100); //10
	int iSliderValue3 =3;
	createTrackbar("smooth", "configurazione", &iSliderValue3, 100); //8
	
	int iSliderValue5 = 100;
	createTrackbar("distanza", "configurazione", &iSliderValue5, 1000); // 200
	int iSliderValue6 = 100;
	createTrackbar("larghezza/altezza max", "configurazione", &iSliderValue6, 200); //100
	


	VideoCapture capture;
	VideoCapture capturebase;
	char a;
	
		cout << "scegliere la modalita'live o video.\n Digitare l (live) o v (video).\n";
		a = cin.get();
	
	
	if (a == 'l') {
		cout << "si e' scelta la modalita'live.\n";
		capture.open(0);
		capturebase.open(0);
	}
	else if (a == 'v') {
		cout << "si e' scelta la modalita'video.\n";
		capture.open("video6.avi");
		capturebase.open("video6.avi");
	
	}

	//Set properties  
	int askFileTypeBox = 0; //-1 is show box of codec  
	int Color = 1;
	Size S = Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH),
		(int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));
	VideoWriter out;
	out.open(".\\outVideo.avi", askFileTypeBox, capture.get(CV_CAP_PROP_FPS), S, Color);
	//immagine di base
	Mat img;
	//leggi immagine
	if (capture.isOpened())
		capture.read(img);
	//get info
	int width = img.size.p[1];
	int height = img.size.p[0];
	//temp images
	Mat diff1(img.size(), CV_8UC3);
	Mat diff2(img.size(), CV_8UC3);
	//color images
	Mat movav(img.size(), CV_32FC3);
	//
	bool first = true;
	int n_oggetti = 0;
	//init prt to contours
	CvMemStorage*    memContours = cvCreateMemStorage(0);
	CvSeq*           contours    = nullptr;
	//vector< Vec4i > hierarchy;
	
	vector< std::shared_ptr<Persona> > Oggetti;
	vector< Vec2d > points;

	//p tmp
	std::shared_ptr<Persona> p = nullptr;
	//
	while (capture.isOpened())
	{
		if (img.empty()) break;
		//copia
		Mat imgCopy;
		img.copyTo(imgCopy);
		//calcolo i param di smussamento
		int odd=(getTrackbarPos("smooth", "configurazione") * 2 + 1);
		//applicca sfumatura gaussiana
		GaussianBlur(img, img, Size(odd, odd), 0);
		//filter
		
		accumulateWeighted(img, movav, 0.022);
		//get image
		Mat temp,diff, difforig;
		//convert 
		movav.convertTo(temp, img.type());
		//get diff
		absdiff(img, temp, diff);
		//copy
		diff.copyTo(difforig);
		//gray scale
		cvtColor(diff, diff1, CV_RGB2GRAY);
		blur(diff1, diff1, Size(3, 3));
		//soglia minima per la conversione richiama il valore che ho inserito all'inizio
		double retval = threshold(diff1, diff2, 100, 1000, CV_THRESH_BINARY);
		//dilata l'immagine
		Dilation(3, diff2, diff2, 2, getTrackbarPos("dilate", "configurazione"));
		// elimina le figure sotto un certo valore
		Erosion(3, diff2, diff2, 2, getTrackbarPos("erode", "configurazione"));
		Mat temp1;
		diff2.copyTo(temp1);

		/// Detect edges using canny
		//Canny(diff2, temp1, getTrackbarPos("threshold", "configurazione"), getTrackbarPos("threshold", "configurazione") * 2, 3);
		/// Find contours
		//findContours(temp1, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE); // funziona ma alla fine non vengono deallocati i punti genera un exp
		//cvFindContours()
		cvFindContours(&IplImage(temp1),
					   memContours,
					   &contours,
					   sizeof(CvContour),
					   /*hierarchy,*/ 
					   CV_RETR_EXTERNAL, 
			           CV_CHAIN_APPROX_NONE,
						cvPoint(0, 0));

		for (auto& ogg : Oggetti)
		{

			ogg->bool_attivo = true;
			int AreaBlob = ogg->height*ogg->width;
			//# calcola l'area media del rettangolo dell'oggetto degli ultimi frames
			if (AreaBlob < (ogg->area_media*1.5) && ogg->grande && ogg->presenza > 10)
			{
				//# se l'oggetto e' nello stato piccolo torna nello stato normale, in questo caso l'oggetto da fermo ha ripreso a camminare   
				ogg->grande = false;
			}
			else if (AreaBlob > (ogg->AreaMedia() * 2) || ogg->grande && ogg->presenza > 10) // in questo l'oggetto il blob gigante creato dalla collisione esplode e l'oggetto puo' ritornare allo stato normale
			{	
				ogg->disattivata += 1;
				ogg->grande = true;
			}
			else if (ogg->CalcolaCentro()[0] < 275 || ogg->CalcolaCentro()[0]>600)
			{
				//controlla che l'oggetto sia in un'area ben precisa(l'area di movimento dei passanti)
					ogg->disattivata += 1;
			}
			else if (sqrt(pow(ogg->VecDirection()[0], 2) + pow(ogg->VecDirection()[1], 2)) >= 3)
			{   //# controllo che serve a determinare se l'oggetto si e' fermato oppure non viene disegnato per via di qualche errore o per collisione
				ogg->disattivata += 1;
			}
		}
		
		int i = -1;
		int ivicino = -1;
		for (CvSeq* seqcontours = contours;
			 seqcontours != 0; 
			 seqcontours = seqcontours->h_next)
		{
			//next id persona
			++i;
			//...
			auto bound_rect = cvBoundingRect(seqcontours); //#calcola le coordinate

			Vec2f pt1 (bound_rect.x, bound_rect.y);
			Vec2f pt2 (bound_rect.x + bound_rect.width, bound_rect.y + bound_rect.height);


			float area = (pt2[0] - pt1[0])*(pt2[1] - pt1[1]);
			int larghezza = (pt2[0] - pt1[0]);
			int altezza = (pt2[1] - pt1[1]);
		

			if (larghezza / altezza <= (getTrackbarPos("larghezza/altezza max", "configurazione") / 100.0) && area > 5000)
				
			
			{
				points.push_back(pt1);// #aggiungo un elemento alla lista
				points.push_back(pt2);

				auto cx = pt1[0] + ((pt2[0] - pt1[0]) / 2);
				auto cy = pt1[1] + ((pt2[1] - pt1[1]) / 2);
				
				
				int j = 0;
				int oggetti_attivi = 0;
				Vec2i soglia_distanza (-1, getTrackbarPos("distanza", "configurazione"));
				// #se non c'è la persona, la alloco e l'aggiungo alla lista
				if (!p.get())
				{
					
					p = std::make_shared< Persona >(i);
					Oggetti.push_back(p);
				}
				//cerca il più vicino
				for (auto& ogg : Oggetti)
				{
					if (ogg->bool_attivo && ogg->disattivata <= 30)
					{
						oggetti_attivi += 1;
						Vec2f po = ogg->CalcolaCentro();
						Vec2f vel = ogg->VecDirection();

						if (ogg->presenza > 15)
						{

							//#disegna il centro dell'oggetto
							rectangle(imgCopy, { (int)cx, (int)cy }, { (int)cx, (int)cy }, { 255, 255, 255 }, 7);
						}
						float dist = (sqrt(pow(po[0] + (vel[0] * ogg->disattivata) - cx, 2) + 
							                pow(po[1] + (vel[1] * ogg->disattivata) - cy, 2)));

						if (dist < soglia_distanza[1]) 
						{
							soglia_distanza[0] = j;
							
							soglia_distanza[1] = dist;
						}
					}
					++j;

				}

				if (soglia_distanza[0] != -1) // se non ho trovato manco una persona
				{
					ivicino = soglia_distanza[0];
					
				}
				else 
				{
					//###### se nn c'e' un rettangolo vecchio da associare a un rettangolo nuovo
					ivicino = Oggetti.size();
					p = std::make_shared< Persona >(i);
					Oggetti.push_back(p);
					
				}
				
				Oggetti[ivicino]->bool_attivo = false;
				Oggetti[ivicino]->TrackingPersona(pt1, pt2);
				Oggetti[ivicino]->disattivata = 0;
				Oggetti[ivicino]->presenza += 1;      
				//disegna
				rectangle(imgCopy,
					Point{ (int)pt1[0],  (int)pt1[1] },
					Point{ (int)pt2[0],  (int)pt2[1] },
					Oggetti[ivicino]->color,
					5);
				//diff temp
				double diff_temp = (difftime(time(nullptr) , Oggetti[ivicino]->tmpcreato));
				//#print(Oggetti[i], Oggetti[i].pcontata, diff);
				//#se la corrente persona (oggetto di i) sta da n tempo sullos schermo, allora contala :
				if (!Oggetti[ivicino]->pcontata && 1.500 <= diff_temp && 2.900 >= diff_temp)  
					// && Oggetti[ivicino]->presenza > capture.get(int(CV_CAP_PROP_FPS)*2)) // capture.get = frame rate del video
				{
					Oggetti[ivicino]->pcontata = true;
					n_oggetti += 1;
					
					
				}
			}
		}
		//show
		if (waitKey(15) == 27) { break; }
		//tampa path oggetti

		//stampa numer persone
		scrivi(&IplImage(imgCopy), to_string(n_oggetti));
		//print
		imshow("camera", diff2);
		imshow("camera2", imgCopy);
		out.write(imgCopy);
		//next image
		capture.read(img);
	}

	destroyWindow("camera");
	destroyWindow("camera2");
	destroyWindow("configurazione");
	cout << "il numero di persone e':" + to_string(n_oggetti);


	return 0;
}


/*NOTA:
&operatore di puntamento
serve per generare l'indirizzo di una entità (&entità) 
* operatore di indirizzamento
serve per accedere all'entità tramite il puntatore (*puntatore) 
->operatore "freccia"
serve per accedere ai campi di un record tramite puntatore
** operatore di doppio indirizzamento
serve per la doppia indirezione(**puntatore a puntatore)
*/