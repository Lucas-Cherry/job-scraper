#include <fstream>
#include <cpr/cpr.h>
#include <gumbo.h>
#include <string>

std::ofstream oferty("oferty_pracy.csv"); //Plik csv do ktorego trafia linki do ofert pracy
int licznik = 0; //Z powodu ze kazda oferta ma 2 linki zamiast 1 musze uzyc licznika aby unikac duplikatow

std::string extract_html() { //Zdobywa zawartosc storny w string za pomoca cpr
	cpr::Url adres = cpr::Url{ "https://www.pracuj.pl/praca/bialystok;wp?rd=30&cc=5015%2C5016&et=17&tc=0" };
	cpr::Response odpowiedz = cpr::Get(adres);
	return odpowiedz.text;
}

void szukaj_link_praca(GumboNode* node) { //Funkcja do wypisania linkow do pliky tekstowego
	if (node->type != GUMBO_NODE_ELEMENT) return;

	if (node->v.element.tag == GUMBO_TAG_A) {

		GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href"); //Znajduje linki

		if (href) {
			std::string link = href->value;
			if (link.rfind("oferta") != std::string::npos) {  //Znajduje oferty| 
				if (licznik%2==0) oferty << link << std::endl; //Licznik do
				licznik++;                                    //unikania duplikatow
			}	

		}
	}


	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; i++) {
		szukaj_link_praca(static_cast<GumboNode*>(children->data[i]));
	}
}


int main() {
	std::string zawartosc = extract_html();

	GumboOutput* zawartosc_gumbo = gumbo_parse(zawartosc.c_str());

	szukaj_link_praca(zawartosc_gumbo->root);
	
	oferty.close();

	gumbo_destroy_output(&kGumboDefaultOptions, zawartosc_gumbo);

	return 0;
}