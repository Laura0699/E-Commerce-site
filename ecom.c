#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ID_PRODOTTO_SIZE 6
#define NOME_PRODOTTO_SIZE 20
#define NOME_UTENTE_SIZE 20
#define COGNOME_UTENTE_SIZE 20
#define PSW_UTENTE_SIZE 20
#define EMAIL_UTENTE_SIZE 25

typedef struct prodotto_t
{
	char id[ID_PRODOTTO_SIZE];
	char nome[NOME_PRODOTTO_SIZE];
	int tipo;
	float prezzo;
	int disponibilita;
} prodotto;

typedef struct utente_t
{
	char nome[NOME_UTENTE_SIZE];
	char cognome[COGNOME_UTENTE_SIZE];
	char psw[PSW_UTENTE_SIZE];
	char email[EMAIL_UTENTE_SIZE];
	int online;
	int admin;
} utente;

typedef struct nodo_prodotto_t
{
	prodotto prdt;
	struct nodo_prodotto_t *next;
} nodo_prodotto;

typedef struct nodo_tipo_prodotto_t
{
	int tipo;
	nodo_prodotto* lst_prdt;
	struct nodo_tipo_prodotto_t* next;
} nodo_t_prodotto;

typedef struct nodo_utente_t
{
	utente usr;
	nodo_prodotto *history_lst;
	nodo_prodotto *shop_lst;
	struct nodo_utente_t *next;
} nodo_utente;

nodo_utente* inserisci_utente(nodo_utente*, nodo_utente*);
nodo_prodotto* inserisci_prodotto(nodo_prodotto*, nodo_prodotto*);
nodo_utente* alloca_nodo_utente(utente);
nodo_prodotto* alloca_nodo_prodotto(prodotto);
nodo_utente* cancella_utente(nodo_utente*, char*, int *);
nodo_prodotto* cancella_prodotto(nodo_prodotto**, char*);
void stampa_utenti(nodo_utente*);
void scrivi_lista_utenti(nodo_utente*);
void stampa_prodotti(nodo_prodotto*);
void scrivi_magazzino(nodo_t_prodotto*);
utente registra_utente();
prodotto registra_prodotto();
nodo_t_prodotto* carica_magazzino(nodo_t_prodotto*);
nodo_utente* carica_utenti();
nodo_prodotto** get_lista_prdt(nodo_t_prodotto*, int);
nodo_t_prodotto* alloca_tipo(nodo_t_prodotto*, int);
void stampa_magazzino(nodo_t_prodotto*);
int gia_registrato(nodo_utente*, char*);
int online(nodo_utente*, char*);
int rendi_admin(nodo_utente*, char*);	
int rimuovi_admin(nodo_utente*, char*);	
prodotto blocca_prdt(nodo_t_prodotto*, char*, int);
int verifica_disp(nodo_t_prodotto*, char*);
int menu_1();
int menu_2();
int menu_3();
int sub_menu(int);
int controlla_credenziali(nodo_utente*, utente*);
void inserisci_nel_carrello(nodo_utente*, char*, nodo_prodotto*);
nodo_utente* get_nodo_utente(nodo_utente*, char*);
int get_tipo_prodotto(nodo_t_prodotto*, char*);
int prodotto_esiste(nodo_prodotto*, char*);
void rifornisci_prodotto(nodo_prodotto*, char*, int);
nodo_prodotto* rimuovi_dal_carrello(nodo_prodotto**, char*, int);
float acquista_prodotti(nodo_utente*);
void svuota_carrello(nodo_utente*);
void carica_carrelli(nodo_utente*);
void scrivi_carrello(nodo_prodotto*, char*, int);
char* id2file(char*, int);
void dealloca_lista_prdt(nodo_prodotto*);
void dealloca_mgz(nodo_t_prodotto*);

int main()
{
	printf("E-Comm Prj\n");
	int err = 0, logout = 0, tipo;
	int res = 0;
	nodo_utente* lista_usr = NULL;
	nodo_utente* temp_usr = NULL;
	nodo_t_prodotto* mgz = NULL;
	nodo_prodotto* nodo_prdt = NULL;
	nodo_prodotto** dn_prdt = NULL;
	utente usr_t;
	prodotto prdt;
	lista_usr = carica_utenti(lista_usr);
	mgz = carica_magazzino(mgz);
	carica_carrelli(lista_usr);
	char id[ID_PRODOTTO_SIZE];
	int count = 0;
	utente usr;
	do
	{
		do
		{
			switch ((menu_1()))
			{
			case 0:
				return 0;
			case 2:
				usr = registra_utente();
				if(!gia_registrato(lista_usr, usr.email))
				{
					usr.online = 1;
					err = 0;
					lista_usr = carica_utenti(lista_usr);
					lista_usr = inserisci_utente(lista_usr, alloca_nodo_utente(usr));
					printf("\nUTENTE REGISTRATO CON SUCCESSO!\n\n");
					scrivi_lista_utenti(lista_usr);
				} else
				{
					printf("Questa e-mail e' stata gia' utilizzata da un altro account..\n");
					continue;
				}
				break;
			case 1:
				lista_usr = carica_utenti(lista_usr);
				err = controlla_credenziali(lista_usr, &usr);
				if(err != 0)
					printf("Credenziali errate..\n");
				else
				{
					err = online(lista_usr, usr.email);
					if(err != 0)
						printf("Utente gia' connesso..\n");
				}
				if(err != 0) break;
				printf("\n>Login effettuato!\n\n");
				err = 0;
				get_nodo_utente(lista_usr, usr.email)->usr.online = 1;
				scrivi_lista_utenti(lista_usr);
				break;
			}
		} while(err != 0);
		do
		{
			logout = 0;
			switch (sub_menu(get_nodo_utente(lista_usr, usr.email)->usr.admin))
			{
			case 0:
				lista_usr = carica_utenti(lista_usr);
				get_nodo_utente(lista_usr, usr.email)->usr.online = 0;
				scrivi_lista_utenti(lista_usr);
				logout = 1;
				return 0;
			case 1:
				printf("Articoli in Magazzino:\n");
				mgz = carica_magazzino(mgz);
				stampa_magazzino(mgz);
				printf("Seleziona ID da aggiungere al carrello: ");
				scanf("%s", id);
				printf("Inserisci quantita': ");
				scanf("%d", &count);
				if(count <= 0 || count > verifica_disp(mgz, id))
				{
					printf("\n>Quantita' non valida..\n\n");
					break;
				}
				if(!verifica_disp(mgz, id))
				{
					printf("\n>Prodotto non disponibile..\n\n");
					break;
				}
				mgz = carica_magazzino(mgz);
				prdt = blocca_prdt(mgz, id, count);
				scrivi_magazzino(mgz);
				nodo_prdt = (nodo_prodotto*)malloc(sizeof(nodo_prodotto));
				memcpy(&nodo_prdt->prdt, &prdt, sizeof(prodotto));
				carica_carrelli(lista_usr);
				inserisci_nel_carrello(lista_usr, usr.email, nodo_prdt);
				scrivi_carrello(get_nodo_utente(lista_usr, usr.email)->shop_lst, usr.email, 0);
				break;
			case 2:
				carica_carrelli(lista_usr);
				stampa_prodotti(get_nodo_utente(lista_usr, usr.email)->shop_lst);
				printf("Inserisci ID del prodotto da rimuovere: ");
				scanf("%s", prdt.id);
				printf("Specificare numero di prodotti da rimuovere: ");
				scanf("%d", &prdt.disponibilita);
				if(prdt.disponibilita <= 0)
				{
					printf("\nQuantita non valida..\n\n");
					break;
				}
				if((nodo_prdt = rimuovi_dal_carrello(&get_nodo_utente(lista_usr, usr.email)->shop_lst, prdt.id, prdt.disponibilita)) == NULL)
				{
					printf("\nProdotto specificato non valido..\n\n");
					break;
				}
				mgz = carica_magazzino(mgz);
				dn_prdt = get_lista_prdt(mgz, prdt.tipo);
				if(dn_prdt == NULL){
					mgz = alloca_tipo(mgz, prdt.tipo);
					dn_prdt = get_lista_prdt(mgz, prdt.tipo);
					*dn_prdt = inserisci_prodotto(*dn_prdt, nodo_prdt);
				} else
				{
					if(!prodotto_esiste(*dn_prdt, nodo_prdt->prdt.id))
					{
						*dn_prdt = inserisci_prodotto(*dn_prdt, nodo_prdt);
					} else
					{
						rifornisci_prodotto(*dn_prdt, nodo_prdt->prdt.id, prdt.disponibilita);
					}
				}
				scrivi_magazzino(mgz);
				scrivi_carrello(get_nodo_utente(lista_usr, usr.email)->shop_lst, usr.email, 0);
				break;
			case 3:
				carica_carrelli(lista_usr);
				stampa_prodotti(get_nodo_utente(lista_usr, usr.email)->shop_lst);
				break;
			case 4:
				carica_carrelli(lista_usr);
				stampa_prodotti(get_nodo_utente(lista_usr, usr.email)->history_lst);
				break;
			case 5:
				if(get_nodo_utente(lista_usr, usr.email)->shop_lst == NULL)
				{
					printf("\nNessun prodotto nel carrello!\n\n");
					break;
				}
				else
				{
					printf("\nPrezzo Totale: %.2f\n\n", acquista_prodotti(get_nodo_utente(lista_usr, usr.email)));
					svuota_carrello(get_nodo_utente(lista_usr, usr.email));
					scrivi_carrello(get_nodo_utente(lista_usr, usr.email)->shop_lst, usr.email, 0);
					scrivi_carrello(get_nodo_utente(lista_usr, usr.email)->history_lst, usr.email, 1);
				}
				break;
			case 6:
				lista_usr = carica_utenti(lista_usr);
				lista_usr = cancella_utente(lista_usr, usr.email, &res);
				scrivi_lista_utenti(lista_usr);
				logout = 1;
				break;
			case 7:
				if(get_nodo_utente(lista_usr, usr.email)->usr.admin)
				{
					
					printf("Inserisci email dell'utente da Cancellare: ");
					scanf("%s", usr_t.email);
					if(strcmp(usr_t.email, usr.email) == 0)
					{
						printf("\n>Non puoi cancellare te stesso!\n\n");
						break;
					}
					lista_usr = carica_utenti(lista_usr);
					carica_carrelli(lista_usr);
					temp_usr = get_nodo_utente(lista_usr, usr_t.email);
					lista_usr = cancella_utente(lista_usr, usr_t.email, &res);
					//printf("\nTARGET:\n\n");
					//stampa_prodotti(temp_usr->shop_lst);
					if(res == 1)
					{
						//printf("\nCancellazione avvenuta!\n\n");
						mgz = carica_magazzino(mgz);
						while(temp_usr->shop_lst != NULL)
						{
							//printf("\nRimpolpo prodotto con id: %s\n\n", temp_usr->shop_lst->prdt.id);
							dn_prdt = get_lista_prdt(mgz, temp_usr->shop_lst->prdt.tipo);
							if(dn_prdt == NULL)
							{
								//printf("\nTipo non esistente, alloco.\n\n");
								mgz = alloca_tipo(mgz, temp_usr->shop_lst->prdt.tipo);
								dn_prdt = get_lista_prdt(mgz, temp_usr->shop_lst->prdt.tipo);
								nodo_prdt = (nodo_prodotto*)malloc(sizeof(nodo_prodotto));
								memcpy(nodo_prdt, temp_usr->shop_lst, sizeof(nodo_prodotto));
								*dn_prdt = inserisci_prodotto(*dn_prdt, nodo_prdt);
							}
							else
							{
								if(!prodotto_esiste(*dn_prdt, temp_usr->shop_lst->prdt.id))
								{
									//printf("\nProdotto non esistente, inserisco.\n\n");
									nodo_prdt = (nodo_prodotto*)malloc(sizeof(nodo_prodotto));
									memcpy(nodo_prdt, temp_usr->shop_lst, sizeof(nodo_prodotto));
									*dn_prdt = inserisci_prodotto(*dn_prdt, nodo_prdt);
								} else
								{
									//printf("\nProdotto gia esistente, rifornisco.\n\n");
									rifornisci_prodotto(*dn_prdt, temp_usr->shop_lst->prdt.id, temp_usr->shop_lst->prdt.disponibilita);
									printf("\nDone.\n\n");
								}
							}
							temp_usr->shop_lst = temp_usr->shop_lst->next;
						}
						scrivi_lista_utenti(lista_usr);
						scrivi_magazzino(mgz);
					} else printf("\nErrore cancellazione\n\n");
					
				} else printf("\nNon sei amministratore\n\n");
				break;
			case 8:
				prdt = registra_prodotto();
				mgz = carica_magazzino(mgz);
				if((tipo = get_tipo_prodotto(mgz, prdt.id)) == -1)
				{
					dn_prdt = get_lista_prdt(mgz, prdt.tipo);
					nodo_prdt = alloca_nodo_prodotto(prdt);
					if(dn_prdt == NULL)
					{
						mgz = alloca_tipo(mgz, prdt.tipo);
						dn_prdt = get_lista_prdt(mgz, prdt.tipo);
						*dn_prdt = inserisci_prodotto(*dn_prdt, nodo_prdt);
					}
					else
					{
						if(!prodotto_esiste(*dn_prdt, nodo_prdt->prdt.id))
						{
							*dn_prdt = inserisci_prodotto(*dn_prdt, nodo_prdt);
						}
						else
						{
							rifornisci_prodotto(*dn_prdt, nodo_prdt->prdt.id, prdt.disponibilita);
						}
					}
				} 
				else
				{
					if(tipo == prdt.tipo)
					{
						printf("\nAggiungo rifornimenti a prodotto esistente\n\n");
						dn_prdt = get_lista_prdt(mgz, prdt.tipo);
						rifornisci_prodotto(*dn_prdt, prdt.id, prdt.disponibilita);
					}
					else
					{
						dn_prdt = get_lista_prdt(mgz, tipo);
						rifornisci_prodotto(*dn_prdt, prdt.id, prdt.disponibilita);
						printf("\nID gia esistente, informazioni inserite non coerenti: rifornimeto completato\n\n");
					}
				}
				scrivi_magazzino(mgz);
				break;
			case 9:
				mgz = carica_magazzino(mgz);
				stampa_magazzino(mgz);
				printf("Inserisci ID prodotto da eliminare: ");
				scanf("%s", prdt.id);
				if((tipo = get_tipo_prodotto(mgz, prdt.id)) != -1)
				{
					cancella_prodotto(get_lista_prdt(mgz, tipo), prdt.id);
					scrivi_magazzino(mgz);
				} else printf("\nProdotto non valido..\n\n");
				break;
			case 10:
				lista_usr = carica_utenti(lista_usr);
				carica_carrelli(lista_usr);
				if(get_nodo_utente(lista_usr, usr.email)->usr.admin)
				{
					printf("Inserisci email dell'utente proprietario del carrello: ");
					scanf("%s", usr_t.email);
					if((temp_usr = get_nodo_utente(lista_usr, usr_t.email)) != NULL)
						stampa_prodotti(temp_usr->shop_lst);
					else printf("\nUtente non valido..\n\n");
				} else printf("\nNon sei amministratore\n\n");
				break;
			case 11:
				lista_usr = carica_utenti(lista_usr);
				carica_carrelli(lista_usr);
				if(get_nodo_utente(lista_usr, usr.email)->usr.admin)
				{
					printf("Inserisci email dell'utente proprietario dello storico: ");
					scanf("%s", usr_t.email);
					if((temp_usr = get_nodo_utente(lista_usr, usr_t.email)) != NULL)
						stampa_prodotti(temp_usr->history_lst);
					else printf("\nUtente non valido..\n\n");
				} else printf("\nNon sei amministratore\n\n");
				break;
			case 15:
				lista_usr = carica_utenti(lista_usr);
				carica_carrelli(lista_usr);
				if(get_nodo_utente(lista_usr, usr.email)->usr.admin)
				{
					temp_usr = lista_usr;
					while(temp_usr != NULL)
					{
						if(temp_usr->usr.admin != 1)
						{
							printf("\n>UTENTE CON EMAIL: %s\n", temp_usr->usr.nome);
							stampa_prodotti(temp_usr->history_lst);
						}
							
						temp_usr = temp_usr->next;
					}
				} else printf("\nNon sei amministratore\n\n");
				break;
			case 12:
				lista_usr = carica_utenti(lista_usr);
				if(get_nodo_utente(lista_usr, usr.email)->usr.admin)
				{
					printf("Inserisci email dell'utente da rendere Admin: ");
					scanf("%s", usr_t.email);
					if(!rendi_admin(lista_usr, usr_t.email))
						printf("\n>Utente non valido..\n\n");
						else
						{	
						scrivi_lista_utenti(lista_usr);
						printf("\nOperazione Completata!\n\n");
						}
				} else printf("\nNon sei amministratore\n\n");
				break;
			case 13:
				mgz = carica_magazzino(mgz);
				stampa_magazzino(mgz);
				break;
			case 14:
				lista_usr = carica_utenti(lista_usr);
				get_nodo_utente(lista_usr, usr.email)->usr.online = 0;
				scrivi_lista_utenti(lista_usr);
				logout = 1;
				break;
			}
		} while (logout == 0);
	} while (1);
}

int menu_1()
{
	int c;
	do
	{
		printf("1 - Login\n");
		printf("2 - Registrazione\n");
		printf("0 - Esci\n");
		printf("Scegli: ");
		scanf("%d", &c);
		if(c < 0 || c > 2)
			printf("Scelta non disponibile..\n\n");
	} while(c < 0 || c > 2);
	return c;
}

int menu_2()
{
	int c;
	do
	{
		printf("7 - Cancella Utente registrato\n");
		printf("8 - Inserisci Prodotto\n");
		printf("9 - Elimina Prodotto\n");
		printf("10 - Visualizza Carrello di un utente\n");
		printf("11 - Visualizza Storico di un utente\n");
		printf("15 - Visualizza Storico Generale\n");
		printf("12 - Rendi Admin\n");
		printf("13 - Visualizza Articoli\n");
		printf("14 - Logout\n");
		printf("0 - Esci\n");
		printf("Scegli: ");
		scanf("%d", &c);
		if((c < 7 && c != 0) || c > 15)
			printf("Scelta non disponibile..\n\n");
	} while ((c < 7 && c != 0) || c > 15);
}

int menu_3()
{
	int c;
	do
	{
		printf("1 - Inserisci nel Carrello\n");
		printf("2 - Rimuovi dal Carrello\n");
		printf("3 - Visualizza Carrello\n");
		printf("4 - Visualizza Storico Acquisti\n");
		printf("5 - Acquista\n");
		printf("6 - Cancella questo Utente\n");
		printf("13 - Visualizza Articoli\n");
		printf("14 - Logout\n");
		printf("0 - Esci\n");
		printf("Scegli: ");
		scanf("%d", &c);
		if(c < 0 || c > 14 || (c > 6 && c < 13)) 
			printf("Scelta non disponibile..\n\n");
	} while (c < 0 || c > 14 || (c > 6 && c < 13));
}

int sub_menu(int is_admin)
{
	if(is_admin)
	{
		return menu_2();
	} else
	{
		return menu_3();
	}
}

void stampa_utenti(nodo_utente* lst_utenti)
{	
	if(lst_utenti == NULL)
	{
		printf("\n>Nessun utente registrato..\n");
		return;
	}
	while(lst_utenti != NULL)
	{
		printf("----\n");
		printf("Nome: %s\n", lst_utenti->usr.nome);
		printf("Cognome: %s\n", lst_utenti->usr.cognome);
		printf("Email: %s\n", lst_utenti->usr.email);
		printf("Password: %s\n", lst_utenti->usr.psw);
		printf("Online: %d\n", lst_utenti->usr.online);
		lst_utenti = lst_utenti->next;
	}
	printf("\n\n");
}

void stampa_prodotti(nodo_prodotto* lst_prdt)
{
	if(lst_prdt == NULL)
	{
		printf("\n>Nessun prodotto registrato..\n\n");
		return;
	}
	while(lst_prdt != NULL)
	{
		printf("----\n");
		printf("ID: %s\n", lst_prdt->prdt.id);
		printf("TIPO: %d\n", lst_prdt->prdt.tipo);
		printf("NOME: %s\n", lst_prdt->prdt.nome);
		printf("PREZZO: %.2f\n", lst_prdt->prdt.prezzo);
		printf("DISPONIBILITA: %d\n", lst_prdt->prdt.disponibilita);
		lst_prdt = lst_prdt->next;
	}
	printf("\n");
}

void stampa_magazzino(nodo_t_prodotto* head)
{
	while(head != NULL)
	{
		printf(">PRODOTTI TIPO: %d\n", head->tipo);
		stampa_prodotti(head->lst_prdt);
		printf("----------------\n");
		head = head->next;
	}
}

utente registra_utente()
{
	utente usr;
	utente admin;
	
	memset(usr.psw, '\0', PSW_UTENTE_SIZE*sizeof(char));
	memset(usr.email, '\0', EMAIL_UTENTE_SIZE*sizeof(char));
	memset(admin.email, '\0', EMAIL_UTENTE_SIZE*sizeof(char));
	memcpy(admin.email, "admin", 5*sizeof(char));
	memset(usr.nome, '\0', NOME_UTENTE_SIZE*sizeof(char));
	memset(usr.cognome, '\0', COGNOME_UTENTE_SIZE*sizeof(char));
	usr.admin = 0;
	usr.online = 0;
	printf("Inserisci nome: ");
	scanf("%s", usr.nome);
	printf("Inserisci cognome: ");
	scanf("%s", usr.cognome);
	printf("Inserisci email: ");
	scanf("%s", usr.email);
	printf("Inserisci password: ");
	scanf("%s", usr.psw);
	if(strcmp(admin.email, usr.email) == 0) usr.admin = 1;
	return usr;
}

prodotto registra_prodotto()
{
	prodotto prdt;
	memset(prdt.id, '\0', ID_PRODOTTO_SIZE*sizeof(char));
	memset(prdt.nome, '\0', NOME_PRODOTTO_SIZE*sizeof(char));
	printf("Inserisci ID prodotto: ");
	scanf("%s", prdt.id);
	printf("Inserisci TIPO prodotto: ");
	scanf("%d", &prdt.tipo);
	printf("Inserisci NOME prodotto: ");
	scanf("%s", prdt.nome);
	printf("Inserisci PREZZO prodotto: ");
	scanf("%f", &prdt.prezzo);
	printf("Inserisci DISPONIBILITA' prodotto: ");
	scanf("%d", &prdt.disponibilita);
	return prdt;
}

int controlla_credenziali(nodo_utente* head, utente* usr)
{
	printf("Inserisci EMAIL: ");
	scanf("%s", usr->email);
	printf("Inserisci PASSWORD: ");
	scanf("%s", usr->psw);
	while(head != NULL)
	{
		if((strcmp(head->usr.email, usr->email)) == 0)
		{
			if((strcmp(head->usr.psw, usr->psw)) == 0)
				return 0;
			else
				return 1;
		}
		head = head->next;
	}
	return -1;
}

nodo_utente* inserisci_utente(nodo_utente* head, nodo_utente* usr)
{
	nodo_utente* copia_head = head;
	nodo_utente* temp = head;
	if(head == NULL)
	{
		head = usr;
		head->next = NULL;
	} else
	{
		while((copia_head != NULL) && (strcmp(copia_head->usr.email, usr->usr.email) < 0))
		{
			temp = copia_head;
			copia_head = copia_head->next;
		}
		if(temp != copia_head)
		{
			usr->next = copia_head;
			temp->next = usr;
		} 
		else
		{
			usr->next = head;
			head = usr;
		}
		//usr->next = head;
		//head = usr;
	}
	return head;
}

nodo_prodotto* inserisci_prodotto(nodo_prodotto* head, nodo_prodotto* prdt)
{
	nodo_prodotto* copia_head = head;
	nodo_prodotto* temp = head;
	
	if(head == NULL)
	{
		head = prdt;
		head->next = NULL;
	} else
	{
		while((copia_head != NULL) && (strcmp(copia_head->prdt.id, prdt->prdt.id) < 0))
		{
			temp = copia_head;
			copia_head = copia_head->next;
		}
		if(temp != copia_head)
		{
			prdt->next = copia_head;
			temp->next = prdt;
		} 
		else
		{
			prdt->next = head;
			head = prdt;
		}
		//prdt->next = head;
		//head = prdt;
		
	}
	return head;
}

nodo_utente* cancella_utente(nodo_utente* lst_utenti, char* e, int* res)
{
	*res = 0;
	nodo_utente* temp = lst_utenti;
	nodo_utente* copia_head = lst_utenti;
	char* email = (char*)malloc(sizeof(char)*EMAIL_UTENTE_SIZE);
	memset(email, '\0', EMAIL_UTENTE_SIZE*sizeof(char));
	memcpy(email, e, EMAIL_UTENTE_SIZE*sizeof(char));
	if(lst_utenti == NULL) return NULL;
	if(lst_utenti->next == NULL)
	{
		if(strcmp(lst_utenti->usr.email, email) == 0 && lst_utenti->usr.admin != 1)
		{
			*res = 1;
			return NULL;
		}
	}
	while(lst_utenti != NULL)
	{
		//printf("[%s] [%s] [%d]\n", lst_utenti->usr.email, email, memcmp(lst_utenti->usr.email, email, EMAIL_UTENTE_SIZE*sizeof(char)));
		if(strcmp(lst_utenti->usr.email, email) == 0 && lst_utenti->usr.admin != 1)
		{
			if(temp == lst_utenti)
			{
				copia_head = temp->next;
			} else
			{
				temp->next = lst_utenti->next;
			}
			remove(id2file(lst_utenti->usr.email, 0));
			remove(id2file(lst_utenti->usr.email, 1));
			*res = 1;
			break;
		}
		temp = lst_utenti;
		lst_utenti = lst_utenti->next;
	}
	return copia_head;
}

nodo_prodotto* cancella_prodotto(nodo_prodotto** d_ptr, char* i)
{
	nodo_prodotto* head = *d_ptr;
	nodo_prodotto* temp = head;
	nodo_prodotto* node_prdt = NULL;
	char* id = (char*)malloc(sizeof(char)*ID_PRODOTTO_SIZE);
	memset(id, '\0', ID_PRODOTTO_SIZE*sizeof(char));
	memcpy(id, i, ID_PRODOTTO_SIZE*sizeof(char));
	if(head == NULL) return NULL;
	if(head->next == NULL)
	{
		if(strcmp(head->prdt.id, id) == 0) 
		{
			*d_ptr = NULL;
			return head;
		}
	}
	while(head != NULL)
	{
		if(strcmp(head->prdt.id, id) == 0)
		{
			if(temp == head)
			{
				*d_ptr = temp->next;
				temp->next = NULL;
				return temp;
			} else
			{
				temp->next = head->next;
				head->next = NULL;
				return head;
			}
		}
		temp = head;
		head = head->next;
	}
	return node_prdt;
}

nodo_prodotto* rimuovi_dal_carrello(nodo_prodotto** d_ptr, char* i, int s)
{
	nodo_prodotto* head = *d_ptr;
	nodo_prodotto* temp = head;
	nodo_prodotto* node_prdt = NULL;
	char* id = (char*)malloc(sizeof(char)*ID_PRODOTTO_SIZE);
	memset(id, '\0', ID_PRODOTTO_SIZE*sizeof(char));
	memcpy(id, i, ID_PRODOTTO_SIZE*sizeof(char));
	if(head == NULL) return NULL;
	if(head->next == NULL)
	{
		if(strcmp(head->prdt.id, id) == 0) 
		{
			if(head->prdt.disponibilita - s <= 0){
				*d_ptr = NULL;
				return head;
			} else
			{
				head->prdt.disponibilita -= s;
				node_prdt = (nodo_prodotto*)malloc(sizeof(nodo_prodotto));
				memcpy(&node_prdt->prdt, &head->prdt, sizeof(prodotto));
				node_prdt->prdt.disponibilita = s;
				return node_prdt;
			}
		}
	}
	while(head != NULL)
	{
		if(strcmp(head->prdt.id, id) == 0)
		{
			if(temp == head)
			{
				if(head->prdt.disponibilita - s <= 0){
					*d_ptr = temp->next;
					temp->next = NULL;
					return temp;
				} else
				{
					head->prdt.disponibilita -= s;
					node_prdt = (nodo_prodotto*)malloc(sizeof(nodo_prodotto));
					memcpy(&node_prdt->prdt, &head->prdt, sizeof(prodotto));
					node_prdt->prdt.disponibilita = s;
					return node_prdt;
				}
			} else
			{
				if(head->prdt.disponibilita - s <= 0){
					temp->next = head->next;
					head->next = NULL;
					return head;
				} else
				{
					head->prdt.disponibilita -= s;
					node_prdt = (nodo_prodotto*)malloc(sizeof(nodo_prodotto));
					memcpy(&node_prdt->prdt, &head->prdt, sizeof(prodotto));
					node_prdt->prdt.disponibilita = s;
					return node_prdt;
				}
			}
		}
		temp = head;
		head = head->next;
	}
}


nodo_prodotto* alloca_nodo_prodotto(prodotto str_prdt)
{
	nodo_prodotto* nodo_prdt = (nodo_prodotto*)malloc(sizeof(nodo_prodotto));
	memcpy(&nodo_prdt->prdt, &str_prdt, sizeof(prodotto));
	nodo_prdt->next = NULL;
	return nodo_prdt;
}

nodo_utente* alloca_nodo_utente(utente str_usr)
{
	nodo_utente* nodo_usr = (nodo_utente*)malloc(sizeof(nodo_utente));
	nodo_usr->history_lst = NULL;
	nodo_usr->shop_lst = NULL;
	nodo_usr->next = NULL;
	memcpy(&nodo_usr->usr, &str_usr, sizeof(utente));
	return nodo_usr;
}

nodo_t_prodotto* carica_magazzino(nodo_t_prodotto* d)
{
	FILE *fp;
	prodotto prdt;
	nodo_t_prodotto* mgz = NULL;
	nodo_prodotto* node_prdt = NULL;
	nodo_prodotto** lista_tipo = NULL;
	dealloca_mgz(d);
	if((fp = fopen("magazzino.txt", "r")) == NULL) return NULL;
	while((fscanf(fp, "%s %d %s %f %d", prdt.id, &prdt.tipo, prdt.nome, &prdt.prezzo, &prdt.disponibilita)) != EOF)
	{
		node_prdt = (nodo_prodotto*)malloc(sizeof(nodo_prodotto));
		memcpy(&node_prdt->prdt, &prdt, sizeof(prodotto));
		
		if((lista_tipo = get_lista_prdt(mgz, prdt.tipo)) != NULL)
		{
			*lista_tipo = inserisci_prodotto(*lista_tipo, node_prdt);
		} else
		{
			mgz = alloca_tipo(mgz, prdt.tipo);
			lista_tipo = get_lista_prdt(mgz, prdt.tipo);
			*lista_tipo = inserisci_prodotto(*lista_tipo, node_prdt);
		}
	}
	fclose(fp);
	return mgz;
}

nodo_prodotto** get_lista_prdt(nodo_t_prodotto* head, int tipo)
{
	while(head != NULL)
	{
		if(head->tipo == tipo)
			return &head->lst_prdt;
		head = head->next;
	}
	return NULL;
}

nodo_t_prodotto* alloca_tipo(nodo_t_prodotto* head, int tipo)
{
	nodo_t_prodotto* nodo_t_prdt = (nodo_t_prodotto*)malloc(sizeof(nodo_t_prodotto));
	nodo_t_prdt->tipo = tipo;
	nodo_t_prdt->lst_prdt = NULL;
	nodo_t_prdt->next = NULL;
	nodo_t_prodotto* copia_head = head;
	nodo_t_prodotto* temp = head;
	
	if(get_lista_prdt(head, tipo) == NULL)
	{
		if(head == NULL)
		{
			head = nodo_t_prdt;
		} else
		{
			while((copia_head != NULL) && (copia_head->tipo < nodo_t_prdt->tipo))
			{
				temp = copia_head;
				copia_head = copia_head->next;
			}
			if(temp != copia_head)
			{
				nodo_t_prdt->next = copia_head;
				temp->next = nodo_t_prdt;
			} 
			else
			{
				nodo_t_prdt->next = head;
				head = nodo_t_prdt;
			}
			//nodo_t_prdt->next = head;
			//head = nodo_t_prdt;
		}
		return head;
	} 
	return head;
}

nodo_utente* carica_utenti()
{
	FILE *fp;
	utente usr;
	nodo_utente* nodo_usr = NULL;
	nodo_utente* head = NULL;
	if((fp = fopen("utenti.txt", "r")) == NULL) return NULL;
	while((fscanf(fp, "%s %s %s %s %d %d", usr.nome, usr.cognome, usr.email, usr.psw, &usr.admin, &usr.online)) != EOF)
	{
		nodo_usr = (nodo_utente*)malloc(sizeof(nodo_utente));
		memcpy(&nodo_usr->usr, &usr, sizeof(utente));
		nodo_usr->shop_lst = NULL;
		nodo_usr->history_lst = NULL;
		head = inserisci_utente(head, nodo_usr);
	}
	return head;
}

void scrivi_lista_utenti(nodo_utente* head)
{
	FILE *fp;
	fp = fopen("utenti.txt", "w");
	while(head != NULL)
	{
		fprintf(fp, "%s %s %s %s %d %d\n", head->usr.nome, head->usr.cognome, head->usr.email, head->usr.psw, head->usr.admin, head->usr.online);
		head = head->next;
	}
	fclose(fp);
}

int gia_registrato(nodo_utente* head, char* e)
{
	char* email = (char *)malloc(sizeof(char)*EMAIL_UTENTE_SIZE);
	memset(email, '\0', sizeof(char)*EMAIL_UTENTE_SIZE);
	memcpy(email, e, sizeof(char)*EMAIL_UTENTE_SIZE);
	while(head != NULL)
	{
		//printf("Comparing %s with %s\n", head->usr.email, email);
		if(strcmp(head->usr.email, email) == 0) return 1;
		head = head->next;
	}
	return 0;
}

int online(nodo_utente* head, char* e)
{
	char* email = (char *)malloc(sizeof(char)*EMAIL_UTENTE_SIZE);
	memset(email, '\0', sizeof(char)*EMAIL_UTENTE_SIZE);
	memcpy(email, e, sizeof(char)*EMAIL_UTENTE_SIZE);
	while(head != NULL)
	{
		if(strcmp(head->usr.email, email) == 0) 
		{
			if(head->usr.online == 1) return 1;
			else return 0;
		} 
		head = head->next;
	}
	return -1;
}

int rendi_admin(nodo_utente* head, char* e)
{
	char* email = (char *)malloc(sizeof(char)*EMAIL_UTENTE_SIZE);
	memset(email, '\0', sizeof(char)*EMAIL_UTENTE_SIZE);
	memcpy(email, e, sizeof(char)*EMAIL_UTENTE_SIZE);
	while(head != NULL)
	{
		if(strcmp(head->usr.email, email) == 0) 
		{
			head->usr.admin = 1;
			return 1;
		} 
		head = head->next;
	}
	return 0;
}

int rimuovi_admin(nodo_utente* head, char* e)
{
	char* email = (char *)malloc(sizeof(char)*EMAIL_UTENTE_SIZE);
	memset(email, '\0', sizeof(char)*EMAIL_UTENTE_SIZE);
	memcpy(email, e, sizeof(char)*EMAIL_UTENTE_SIZE);
	while(head != NULL)
	{
		if(strcmp(head->usr.email, email) == 0) 
		{
			head->usr.admin = 0;
		} 
		head = head->next;
	}
	return -1;
}

void scrivi_magazzino(nodo_t_prodotto* mgz)
{
	FILE* fp = fopen("magazzino.txt", "w");
	nodo_prodotto* temp;
	while(mgz != NULL)
	{
		temp = mgz->lst_prdt;
		while(temp != NULL)
		{
			fprintf(fp, "%s %d %s %.2f %d\n", temp->prdt.id, temp->prdt.tipo, temp->prdt.nome, temp->prdt.prezzo, temp->prdt.disponibilita);
			temp = temp->next;
		}
		mgz = mgz->next;
	}
	fclose(fp);
}

int verifica_disp(nodo_t_prodotto* head, char* i)
{
	char* id = (char *)malloc(sizeof(char)*ID_PRODOTTO_SIZE);
	memset(id, '\0', sizeof(char)*ID_PRODOTTO_SIZE);
	memcpy(id, i, sizeof(char)*ID_PRODOTTO_SIZE);
	nodo_prodotto* temp;
	while (head != NULL)
	{
		temp = head->lst_prdt;
		while(temp != NULL)
		{
			if(strcmp(temp->prdt.id, id) == 0)
			{
				return temp->prdt.disponibilita;
			}
			temp = temp->next;
		}
		head = head->next;
	}
	return 0;
}

prodotto blocca_prdt(nodo_t_prodotto* head, char* i, int v)
{
	char* id = (char *)malloc(sizeof(char)*ID_PRODOTTO_SIZE);
	memset(id, '\0', sizeof(char)*ID_PRODOTTO_SIZE);
	memcpy(id, i, sizeof(char)*ID_PRODOTTO_SIZE);
	prodotto prdt;
	memset(&prdt, '\0', sizeof(char)*ID_PRODOTTO_SIZE);
	nodo_prodotto* temp;
	while (head != NULL)
	{
		temp = head->lst_prdt;
		while(temp != NULL)
		{
			if(strcmp(temp->prdt.id, id) == 0)
			{
				if((temp->prdt.disponibilita - v) >= 0)
				{
					memcpy(&prdt, &temp->prdt, sizeof(prodotto));
					prdt.disponibilita = v;
					temp->prdt.disponibilita -= v;
					if(temp->prdt.disponibilita == 0)
					{
						cancella_prodotto(&head->lst_prdt, temp->prdt.id);
					}
					return prdt;
				}
			}
			temp = temp->next;
		}
		head = head->next;
	}
	return prdt;
}

void inserisci_nel_carrello(nodo_utente* head, char* e, nodo_prodotto* node_prdt)
{
	char* email = (char *)malloc(sizeof(char)*EMAIL_UTENTE_SIZE);
	memset(email, '\0', sizeof(char)*EMAIL_UTENTE_SIZE);
	memcpy(email, e, sizeof(char)*EMAIL_UTENTE_SIZE);
	nodo_prodotto* temp = NULL;
	while(head != NULL)
	{
		if((strcmp(head->usr.email, email)) == 0)
		{
			temp = head->shop_lst;
			while(temp != NULL)
			{
				if((strcmp(temp->prdt.id, node_prdt->prdt.id)) == 0)
				{
					temp->prdt.disponibilita += node_prdt->prdt.disponibilita;
					return;
				}
				temp = temp->next;
			}
			head->shop_lst = inserisci_prodotto(head->shop_lst, node_prdt);
			return;
		}
		head = head->next;
	}
}

nodo_utente* get_nodo_utente(nodo_utente* head, char* e)
{
	char* email = (char *)malloc(sizeof(char)*EMAIL_UTENTE_SIZE);
	memset(email, '\0', sizeof(char)*EMAIL_UTENTE_SIZE);
	memcpy(email, e, sizeof(char)*EMAIL_UTENTE_SIZE);
	while(head != NULL)
	{
		if((strcmp(head->usr.email, email)) == 0)
		{
			return head;
		}
		head = head->next;
	}
	return NULL;
}

int get_tipo_prodotto(nodo_t_prodotto* head, char* i)
{
	char* id = (char *)malloc(sizeof(char)*ID_PRODOTTO_SIZE);
	memset(id, '\0', sizeof(char)*ID_PRODOTTO_SIZE);
	memcpy(id, i, sizeof(char)*ID_PRODOTTO_SIZE);
	nodo_prodotto* temp = NULL;
	while(head != NULL)
	{
		temp = head->lst_prdt;
		while(temp != NULL)
		{
			if((strcmp(temp->prdt.id, id)) == 0)
			{
				return temp->prdt.tipo;
			}
			temp = temp->next;
		}
		head = head->next;
	}
	return -1;
}

int prodotto_esiste(nodo_prodotto* head, char* i)
{
	char* id = (char *)malloc(sizeof(char)*ID_PRODOTTO_SIZE);
	memset(id, '\0', sizeof(char)*ID_PRODOTTO_SIZE);
	memcpy(id, i, sizeof(char)*ID_PRODOTTO_SIZE);
	while(head != NULL)
	{
		if((strcmp(head->prdt.id, id)) == 0)
		{
			return 1;
		}
		head = head->next;
	}
	return 0;
}


void rifornisci_prodotto(nodo_prodotto* head, char* i, int s)
{
	char* id = (char *)malloc(sizeof(char)*ID_PRODOTTO_SIZE);
	memset(id, '\0', sizeof(char)*ID_PRODOTTO_SIZE);
	memcpy(id, i, sizeof(char)*ID_PRODOTTO_SIZE);
	while(head != NULL)
	{
		if((strcmp(head->prdt.id, id)) == 0)
		{
			head->prdt.disponibilita += s;
			return;
		}
		head = head->next;
	}
}

float acquista_prodotti(nodo_utente* head)
{
	nodo_prodotto* lista_prdt = head->shop_lst;
	float count = 0;
	while(lista_prdt != NULL)
	{
		count += (lista_prdt->prdt.prezzo*lista_prdt->prdt.disponibilita);
		lista_prdt = lista_prdt->next;
	}
	return count;
}

void svuota_carrello(nodo_utente* head)
{
	nodo_prodotto* prdt;
	while(head->shop_lst != NULL)
	{
		prdt = cancella_prodotto(&head->shop_lst, head->shop_lst->prdt.id);
		prdt->next = NULL;
		head->history_lst = inserisci_prodotto(head->history_lst, prdt);
	}
}

char* id2file(char* t, int s)
{
	char* id = (char*)malloc((sizeof(char)*EMAIL_UTENTE_SIZE));
	memset(id, '\0', (sizeof(char)*EMAIL_UTENTE_SIZE));
	memcpy(id, t, (sizeof(char)*EMAIL_UTENTE_SIZE));
	if(!s)
		memcpy(id+(strlen(t)), "S.txt\0", 6);
	else
		memcpy(id+(strlen(t)), "H.txt\0", 6);
	return id;
}

void carica_carrelli(nodo_utente* head)
{
	FILE* fp;
	prodotto prdt;
	nodo_prodotto* node_prdt = NULL;
	while(head != NULL)
	{
		//printf("%s %s\n", head->usr.email, id2file(head->usr.email, 0));
		dealloca_lista_prdt(head->shop_lst);
		head->shop_lst = NULL;
		if((fp = fopen(id2file(head->usr.email, 0), "r")) != NULL)
		{
			while((fscanf(fp, "%s %d %s %f %d", prdt.id, &prdt.tipo, prdt.nome, &prdt.prezzo, &prdt.disponibilita)) != EOF)
			{
				node_prdt = (nodo_prodotto*)malloc(sizeof(nodo_prodotto));
				memcpy(&node_prdt->prdt, &prdt, sizeof(prodotto));
				head->shop_lst = inserisci_prodotto(head->shop_lst, node_prdt);
			}
			fclose(fp);
		}
		dealloca_lista_prdt(head->history_lst);
		head->history_lst = NULL;
		if((fp = fopen(id2file(head->usr.email, 1), "r")) != NULL)
		{
			while((fscanf(fp, "%s %d %s %f %d", prdt.id, &prdt.tipo, prdt.nome, &prdt.prezzo, &prdt.disponibilita)) != EOF)
			{
				node_prdt = (nodo_prodotto*)malloc(sizeof(nodo_prodotto));
				memcpy(&node_prdt->prdt, &prdt, sizeof(prodotto));
				head->history_lst = inserisci_prodotto(head->history_lst, node_prdt);
			}
			fclose(fp);
		}
		head = head->next;
	}
}

void scrivi_carrello(nodo_prodotto* temp, char* n, int s)
{
	FILE* fp = fopen(id2file(n, s), "w");
	if(fp == NULL)
	{
		printf("\nErrore Scritta carrello..\n\n");
		return;
	}
	while(temp != NULL)
	{
		fprintf(fp, "%s %d %s %.2f %d\n", temp->prdt.id, temp->prdt.tipo, temp->prdt.nome, temp->prdt.prezzo, temp->prdt.disponibilita);
		temp = temp->next;
	}
	fclose(fp);
}

void dealloca_lista_prdt(nodo_prodotto* head)
{
	nodo_prodotto* temp = NULL;
	while(head != NULL)
	{
		if(head->next == NULL)
		{
			free(head);
			return;
		}
		temp = head;
		head = head->next;
		free(temp);
	}
}

void dealloca_mgz(nodo_t_prodotto* head)
{
	nodo_t_prodotto* temp = NULL;
	while(head != NULL)
	{
		dealloca_lista_prdt(head->lst_prdt);
		head->lst_prdt = NULL;
		
		if(head->next == NULL)
		{
			free(head);
			return;
		}
		temp = head;
		head = head->next;
		free(temp);
	}
}
