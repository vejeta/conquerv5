
static int
get_cargo(str)
	char *str;
{
	int choice=(-1);

	mvprintw(LINES-3,0,"%s (A)rmy or (P)eople?",str);
	clrtoeol();
	refresh();
	switch(getch()) {
	case 'a':
	case 'A':
	     choice=TRUE;
	     break;
	case 'p':
	case 'P':
	     choice=FALSE;
	     break;
	default:
	     break;
	}

	return(choice);
}

/* this function returns false if loading is invalid */
int
loadstat(status)
	int status;
{
	switch(status) {
	case TRADED:
	case GENERAL:
	case MILITIA:
	case GARRISON:
	case ONBOARD:
		return(FALSE);
		break;
	default:
		break;
	}
	return(TRUE);
}

/* this function loads a fleet with an item */
void
loadfleet()
{
	short nvynum;
	/* merchant holding unused */
	int ghold, mhold, unload, doarmy;
	int gcargo, mcargo, amount, armynum;

	clear_bottom(0);
	if((nvynum=getselunit()-MAXARM)>=0){
		if(nvynum>=MAXNAVY){
			errormsg("Invalid Navy");
			return;
		}
	} else {
		errormsg("Invalid Navy");
		return;
	}

	if(sct[XREAL][YREAL].altitude==WATER) {
		errormsg("Fleet must be landed");
		return;
	}

	/* process loading or unloading */
	ghold = fltghold(nvynum);
	mhold = fltmhold(nvynum);
	if((ghold==0)&&(mhold == 0)) {
		errormsg("No storage space available");
		return;
	} else if((P_NARMY==MAXARM)&&(P_NPEOP==0)) {
		unload=FALSE;
	} else {
		mvprintw(LINES-4,0,"Cargo:   People %d",P_NPEOP*mhold);
		if(P_NARMY==MAXARM) mvaddstr(LINES-4,25,"Army (none)");
		else mvprintw(LINES-4,25,"Army (%d)",P_NARMY);
		mvaddstr(LINES-3,0,"Do you wish to (L)oad or (U)nload?");
		refresh();
		switch(getch()) {
		case 'l':
		case 'L':
		     unload=FALSE;
		     break;
		case 'u':
		case 'U':
		     unload=TRUE;
		     break;
		default:
		     return;
		}
	}

	if(unload==TRUE) {
		if(P_NARMY==MAXARM) doarmy=FALSE;
		else if(P_NPEOP==0) doarmy=TRUE;
		else {
			doarmy=get_cargo("Unload");
		}
		if(doarmy==TRUE) {
			armynum=P_NARMY;
			if(sct[XREAL][YREAL].owner==0
			&& P_ATYPE!=A_MARINES
			&& P_ATYPE!=A_SAILOR) {
				errormsg("Only sailors or marines may disembark in unowned land");
				return;
			} else if (sct[XREAL][YREAL].owner!=country
			&& sct[XREAL][YREAL].owner!=0
			&& P_ATYPE!=A_MARINES) {
				errormsg("Only marines may disembark in someone else's land");
				return;
			}
			P_ASTAT=DEFEND;
			P_NARMY=MAXARM;
			if (!((sct[XREAL][YREAL].designation==DCITY
			|| sct[XREAL][YREAL].designation==DCAPITOL)
			&& (sct[XREAL][YREAL].owner==country
			|| (!ntn[sct[XREAL][YREAL].owner].dstatus[country]!=UNMET
			&& ntn[sct[XREAL][YREAL].owner].dstatus[country]<=NEUTRAL)))
			|| P_NMOVE < N_CITYCOST) {
				P_NMOVE=0;
			} else {
				P_NMOVE-= N_CITYCOST;
			}
			NADJMOV;
			NADJHLD;
			AADJSTAT;
		} else if(doarmy==FALSE){
			if (sct[XREAL][YREAL].owner!=country) {
				mvaddstr(LINES-3,0,"Unload in a sector you don't own? (y or n)");
				clrtoeol();
				refresh();
				if (getch()!='y') {
					return;
				}
			}
			mvaddstr(LINES-2,0,"Unload how many people?");
			refresh();
			amount=get_number();
			if(amount > mhold*P_NPEOP) {
				errormsg("There are not that many on board");
			} else if (amount > 0) {
				sct[XREAL][YREAL].people += amount;
				P_NPEOP=(unsigned char)((mhold*P_NPEOP-amount)/mhold);
				NADJHLD;
				if ((sct[XREAL][YREAL].designation!=DCITY
				&& sct[XREAL][YREAL].designation!=DCAPITOL)
				|| P_NMOVE < N_CITYCOST) {
					P_NMOVE=0;
				} else {
					P_NMOVE-= N_CITYCOST;
				}
				NADJMOV;
				SADJCIV;
			}
		}
	} else {
		clear_bottom(0);
		mcargo = mhold*(SHIPHOLD-P_NPEOP);
		if(P_NARMY==MAXARM) {
			gcargo = ghold*SHIPHOLD;
			mvprintw(LINES-4,0,"Available Space:  %d soldiers  %d people", gcargo, mcargo);
		} else {
			gcargo = 0;
			mvprintw(LINES-4,0,"Available Space:  0 soldiers  %d people", mcargo);
		}
		if(gcargo==0) doarmy=FALSE;
		else if(mcargo==0) doarmy=TRUE;
		else {
			doarmy=get_cargo("Load");
		}
		if(doarmy==TRUE) {
			mvaddstr(LINES-2,0,"Load what army?");
			refresh();
			armynum = get_number();
			if(armynum<0) {
				;
			} else if((armynum>=MAXARM)||(P_ASOLD<=0)
			||(loadstat(P_ASTAT)==FALSE)) {
				errormsg("Invalid Army");
			} else if((P_AXLOC!=XREAL)||(P_AYLOC!=YREAL)) {
				errormsg("Army not in sector");
			} else if(P_ASOLD > gcargo &&
			(P_ATYPE<MINLEADER || P_ATYPE>=MINMONSTER)) {
				errormsg("Army too large for fleet");
			} else {
				P_ASTAT=ONBOARD;
				P_AMOVE=0;
				P_NARMY=armynum;
				if (!((sct[XREAL][YREAL].designation==DCITY
				|| sct[XREAL][YREAL].designation==DCAPITOL)
				&& (sct[XREAL][YREAL].owner==country
				|| (!ntn[sct[XREAL][YREAL].owner].dstatus[country]!=UNMET
				&& ntn[sct[XREAL][YREAL].owner].dstatus[country]<=NEUTRAL)))
				|| P_NMOVE < N_CITYCOST) {
					P_NMOVE=0;
				} else {
					P_NMOVE-= N_CITYCOST;
				}
				NADJMOV;
				NADJHLD;
				AADJMOV;
				AADJSTAT;
			}
		} else if(doarmy==FALSE && mcargo!=0){
			mvaddstr(LINES-2,0,"Load how many people?");
			refresh();
			amount=get_number();
			if(sct[XREAL][YREAL].owner!=country) {
				errormsg("The people refuse to board");
			} else if(amount > mcargo) {
				errormsg("Not enough room on fleet");
			} else if(sct[XREAL][YREAL].people < amount) {
				errormsg("Not enough people in sector");
			} else if (amount > 0) {
				sct[XREAL][YREAL].people -= amount;
				P_NPEOP += (unsigned char)(amount / mhold);
				SADJCIV;
				if ((sct[XREAL][YREAL].designation!=DCITY
				&& sct[XREAL][YREAL].designation!=DCAPITOL)
				|| P_NMOVE < N_CITYCOST) {
					P_NMOVE=0;
				} else {
					P_NMOVE-= N_CITYCOST;
				}
				NADJMOV;
				NADJHLD;
			}
		} else if (mcargo==0) {
			errormsg("No more room onboard fleet");
		}
	}
}
