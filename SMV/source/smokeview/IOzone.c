// $Date$ 
// $Revision$
// $Author$

#include "options.h"
#include <stdio.h>  
#include <stdlib.h> 
#include <string.h>
#include <math.h>
#include "flowfiles.h"
#include "MALLOC.h"
#include "smokeviewvars.h"

// svn revision character string
char IOzone_revision[]="$Revision$";

/* ------------------ getzonesizecsv ------------------------ */

void getzonesizecsv(int *nzonet, int *nroom, int *nfires, int *error){
   device *dv_time,*dv_room,*dv_fire;
   int nr,nf;
   int i;

   
   *error=0;
   dv_time=getdevice("Time");
   if(dv_time==NULL){
     *error=1;
     return;
   }
   nr=0;
   for(i=0;i<ndeviceinfo;i++){
     char room_label[100];
     
     sprintf(room_label,"LLT_%i",i+1);
     dv_room=getdevice(room_label);
     if(dv_room==NULL)break;
     *nzonet=dv_room->nvals;
     nr++;
   }
   *nroom=nr;

   nf=0;
   for(i=0;i<ndeviceinfo;i++){
     char fire_label[100];
     
     sprintf(fire_label,"HRR_%i",i+1);
     dv_fire=getdevice(fire_label);
     if(dv_fire==NULL)break;
     nf++;
   }
   *nfires=nf;
}

/* ------------------ getzonedatacsv ------------------------ */

void getzonedatacsv(int nzonet, int nrooms, int nfires, 
                    float *zonet, float *zoneqfire, 
                    float *zonepr, float *zoneylay,  float *zonetl, float *zonetu,
                    float **zoneodlptr, float **zoneoduptr, 
                    int *error){
  char *zone_labels[]={
    "Time", "s", 
    "ULT_", "C", "LLT_", "C", "HGT_", "m", "PRS_", "Pa",
    "ATARG_", "W/m^2", "FTARG_", "W/m^2",  
    "HRR_", "W", "FHGT_", "m",
    "ULOD_", "1/m", "LLOD_", "1/m"
  };
  int i,ii,iif, use_od=1;
  device *zonet_dev=NULL, **zoneqfire_devs=NULL;
  device **zonepr_devs=NULL, **zoneylay_devs=NULL, **zonetl_devs=NULL, **zonetu_devs=NULL, **zoneodl_devs=NULL, **zoneodu_devs=NULL;
  float *zoneodl, *zoneodu;

  *error=0;
  if(nfires>0){
    NewMemory((void **)&zoneqfire_devs,nfires*sizeof(device *));
  }

  if(nrooms>0){
    NewMemory((void **)&zonepr_devs,nrooms*sizeof(device *));
    NewMemory((void **)&zoneylay_devs,nrooms*sizeof(device *));
    NewMemory((void **)&zonetl_devs,nrooms*sizeof(device *));
    NewMemory((void **)&zonetu_devs,nrooms*sizeof(device *));
    NewMemory((void **)&zoneodl_devs,nrooms*sizeof(device *));
    NewMemory((void **)&zoneodu_devs,nrooms*sizeof(device *));
  }

  zonet_dev=getdevice("Time");
  for(i=0;i<nrooms;i++){
    char label[100];

    sprintf(label,"PRS_%i",i+1);
    zonepr_devs[i]=getdevice(label);
    if(zonepr_devs[i]==NULL||zonepr_devs[i]->nvals!=nzonet){
      *error=1;
      return;
    }

    sprintf(label,"HGT_%i",i+1);
    zoneylay_devs[i]=getdevice(label);
    if(zoneylay_devs[i]==NULL||zoneylay_devs[i]->nvals!=nzonet){
      *error=1;
      return;
    }

    sprintf(label,"LLT_%i",i+1);
    zonetl_devs[i]=getdevice(label);
    if(zonetl_devs[i]==NULL||zonetl_devs[i]->nvals!=nzonet){
      *error=1;
      return;
    }

    sprintf(label,"ULT_%i",i+1);
    zonetu_devs[i]=getdevice(label);
    if(zonetu_devs[i]==NULL||zonetu_devs[i]->nvals!=nzonet){
      *error=1;
      return;
    }

    sprintf(label,"LLOD_%i",i+1);
    zoneodl_devs[i]=getdevice(label);
    if(zoneodl_devs[i]==NULL)use_od=0;

    sprintf(label,"ULOD_%i",i+1);
    zoneodu_devs[i]=getdevice(label);
    if(zoneodu_devs[i]==NULL)use_od=0;
  }
  if(use_od==1){
    NewMemory((void **)&zoneodl     ,nrooms*nzonet*sizeof(float));
    NewMemory((void **)&zoneodu     ,nrooms*nzonet*sizeof(float));
  }
  else{
    zoneodl=NULL;
    zoneodu=NULL;
  }
  *zoneodlptr=zoneodl;
  *zoneoduptr=zoneodu;

  for(i=0;i<nfires;i++){
    char label[100];

    sprintf(label,"HRR_%i",i+1);
    zoneqfire_devs[i]=getdevice(label);
    if(zoneqfire_devs[i]==NULL||zoneqfire_devs[i]->nvals!=nzonet){
      *error=1;
      return;
    }
  }

  ii=0;
  iif=0;
  for(i=0;i<nzonet;i++){
    int j;

    zonet[i]=zonepr_devs[0]->times[i];
    for(j=0;j<nrooms;j++){
      zonepr[ii]=zonepr_devs[j]->vals[i];
      zoneylay[ii]=zoneylay_devs[j]->vals[i];
      zonetl[ii]=zonetl_devs[j]->vals[i];
      zonetu[ii]=zonetu_devs[j]->vals[i];
      if(use_od==1){
        zoneodl[ii]=zoneodl_devs[j]->vals[i];
        zoneodu[ii]=zoneodu_devs[j]->vals[i];
      }
      ii++;
    }
    for(j=0;j<nfires;j++){
      zoneqfire[iif]=zoneqfire_devs[j]->vals[i];
      iif++;
    }
  }

  FREEMEMORY(zoneqfire_devs);
  FREEMEMORY(zonepr_devs);
  FREEMEMORY(zoneylay_devs);
  FREEMEMORY(zonetl_devs);
  FREEMEMORY(zonetu_devs);
  FREEMEMORY(zoneodl_devs);
  FREEMEMORY(zoneodu_devs);
}

/* ------------------ readzone ------------------------ */

void readzone(int ifile, int flag, int *errorcode){
  int n;
  int error,ntotal,i,j,ii;
  int nrooms2,nfires2;
  size_t zonefilelen;
  int fort_unit;
  zonedata *zonei;
  char *file;

  *errorcode=0;

  zonei = zoneinfo + ifile;
  file = zonei->file;
  if(zonei->loaded==0&&flag==UNLOAD)return;
  FREEMEMORY(zonet);
  FREEMEMORY(zoneylay);
  FREEMEMORY(zonetl);
  FREEMEMORY(zonetu);
  FREEMEMORY(zonepr);
  FREEMEMORY(hazardcolor);
  FREEMEMORY(zoneqfire);
  FREEMEMORY(izonetu);
  FREEMEMORY(zoneodl);
  FREEMEMORY(zoneodu);

  if(colorlabelzone!=NULL){
    for(n=0;n<MAXRGB;n++){
      FREEMEMORY(colorlabelzone[n]);
    }
    FREEMEMORY(colorlabelzone);
  }
  CheckMemory;

  activezone=NULL;
  if(flag==UNLOAD){
    zonei->loaded=0;
    zonei->display=0;
    nzonet=0;
    ReadZoneFile=0;
    showzone=0;
    plotstate=getplotstate(DYNAMIC_PLOTS);
    updatetimes();
    updatemenu=1;
    return;
  }
  zonefilelen = strlen(file);
  if(zonei->csv==1){
    read_device_data(zonei->file,LOAD);
    getzonesizecsv(&nzonet,&nrooms2,&nfires2,&error);
  }
  else{
    FORTgetzonesize(file,&nzonet,&nrooms2,&nfires2,&endian,&error,zonefilelen);
  }
  CheckMemory;
  if(error!=0||nrooms!=nrooms2||nzonet==0){
    showzone=0;
    updatetimes();
    ReadZoneFile=0;
    if(nrooms!=nrooms2){
      printf("*** error: number of rooms specified in the smv file (%i)\n",nrooms);
      printf("    not consistent with the number specified in the zone file (%i)\n",nrooms2);
    }
    if(nzonet<=0)printf("*** warning: The file, %s, contains no data\n",file);
    return;
  }
  FREEMEMORY(zonelonglabels);
  FREEMEMORY(zoneshortlabels);
  FREEMEMORY(zoneunits);
  FREEMEMORY(zonelevels);
  if(NewMemory((void **)&zonelonglabels  ,LABELLEN)==0||
     NewMemory((void **)&zoneshortlabels ,LABELLEN)==0||
     NewMemory((void **)&zoneunits       ,LABELLEN)==0||
     NewMemory((void **)&zonelevels      ,nrgb*sizeof(float))==0){
    *errorcode=1;
    return;
  }
  CheckMemory;

  printf("Loading zone data: %s\n",file);

  ntotal = nrooms*nzonet;

  if(ntotal>0){
    FREEMEMORY(zonet); 
    FREEMEMORY(zoneylay); 
    FREEMEMORY(zonetl); 
    FREEMEMORY(zonetu); 
    FREEMEMORY(zonepr);
    FREEMEMORY(zoneodl);
    FREEMEMORY(zoneodu);
    if(NewMemory((void **)&zonet      ,ntotal*sizeof(float))==0||
       NewMemory((void **)&zoneylay   ,ntotal*sizeof(float))==0||
       NewMemory((void **)&zonetu     ,ntotal*sizeof(float))==0||
       NewMemory((void **)&zonetl     ,ntotal*sizeof(float))==0||
       NewMemory((void **)&zonepr     ,ntotal*sizeof(float))==0||
       NewMemory((void **)&hazardcolor,ntotal*sizeof(unsigned char))==0){
      *errorcode=1;
      return;
    }
    if(nfires!=0){
      FREEMEMORY(zoneqfire);
      if(NewMemory((void **)&zoneqfire,nfires*nzonet*sizeof(float))==0){
        *errorcode=1;
        return;
      }
    }
    else{
      zoneqfire=NULL;
    }
    FREEMEMORY(izonetu);
    if(NewMemory((void **)&izonetu,ntotal*sizeof(unsigned char))==0){
      *errorcode=1;
      return;
    }
  }
  else{
    return;
  }
  CheckMemory;
  if(zonei->csv==1){
    getzonedatacsv(nzonet,nrooms, nfires, zonet, zoneqfire,zonepr,zoneylay,zonetl,zonetu,&zoneodl,&zoneodu,&error);
  }
  else{
    FORTgetzonedata(file,&nzonet,&nrooms, &nfires, zonet,zoneqfire,zonepr,zoneylay,zonetl,zonetu,&endian,&error,zonefilelen);
  }
  CheckMemory;
  ii = 0;
  for(i=0;i<nzonet;i++){
    for(j=0;j<nrooms;j++){
      if(zonetu[ii]>=773.0f){
		    hazardcolor[ii]=RED;
      }
      else{
		    if(zonetu[ii]>=323.0){
          if(zoneylay[ii]>1.5){
            hazardcolor[ii]=YELLOW;
          }
          else{
            hazardcolor[ii]=PINK;
          }
        }
		    else{
          if(zoneylay[ii]>2.0){
            hazardcolor[ii]=BLUE;
          }
          else{
            hazardcolor[ii]=GREEN;
          }
        }
      }
      zoneylay[ii]/=xyzmaxdiff;
      ii++;
    }
  }

  ii=0;
  for(i=0;i<nzonet;i++){
    for(j=0;j<nrooms;j++){
      zonetu[ii]-=273.15;
      zonetl[ii]-=273.15;
      ii++;
    }
  }

  printf("computing zone color levels \n");

  // allocate memory for temperature labels

  if(NewMemory((void **)&colorlabelzone,MAXRGB*sizeof(char *))==0){
    *errorcode=1;
    readzone(ifile,UNLOAD,&error);
    return;
  }
  for(n=0;n<MAXRGB;n++){
    colorlabelzone[n]=NULL;
  }
  for(n=0;n<nrgb;n++){
    if(NewMemory((void **)&colorlabelzone[n],11)==0){
      *errorcode=1;
      readzone(ifile,UNLOAD,&error);
      return;
    }
  }

  if(setzonemin==0||setzonemax==0){
    getzonebounds(zonetu,ntotal,setzonemin,&zonemin,setzonemax,&zonemax);
  }
  getZoneColors(zonetu, ntotal, izonetu,&zonemin, &zonemax, nrgb, nrgb_full, 
    colorlabelzone, zonescale, zonelevels256);

  ReadZoneFile=1;
  visZone=1;
  showzone=1;
  zonei->loaded=1;
  zonei->display=1;
  plotstate=getplotstate(DYNAMIC_PLOTS);
  updatetimes();
  updatemenu=1;
  activezone = zoneinfo + ifile;
  IDLE();

}

/* ------------------ fill_roomdata ------------------------ */

void fill_roomdata(int izone_index){
  float *pr0, *tl0, *tu0, *ylay0, *odl0, *odu0;
  int i;
  roomdata *roomi;

  float PAMB=101325;
  float CP=1004.0;
  float R=0.4*CP/1.4;
  float TAMB=273.15+20.0;

  if(ReadZoneFile==0)return;
  pr0 = zonepr + izone_index*nrooms;
  ylay0 = zoneylay + izone_index*nrooms;
  tl0 = zonetl + izone_index*nrooms;
  tu0 = zonetu + izone_index*nrooms;
  if(zoneodl!=NULL)odl0 = zoneodl + izone_index*nrooms;
  if(zoneodu!=NULL)odu0 = zoneodu + izone_index*nrooms;
  for(i=0;i<nrooms;i++){
    roomi = roominfo + i;
    roomi->pfloor=pr0[i];
    roomi->ylay=ylay0[i];
    roomi->tl=tl0[i]+273.15;
    roomi->tu=tu0[i]+273.15;
    roomi->itl=getZoneColor(tl0[i],zonemin,zonemax,nrgb_full);
    roomi->itu=getZoneColor(tu0[i],zonemin,zonemax,nrgb_full);
    roomi->rho_L=PAMB/R/roomi->tl;
    roomi->rho_U=PAMB/R/roomi->tu;
    if(zoneodl!=NULL)roomi->od_L=odl0[i];
    if(zoneodu!=NULL)roomi->od_U=odu0[i];
  }
  roomi=roominfo+nrooms;
  roomi->pfloor=0.0;
  roomi->ylay=99999.0;
  roomi->tl=TAMB;
  roomi->tu=TAMB;
  roomi->itl=getZoneColor(TAMB-273.15,zonemin,zonemax,nrgb_full);
  roomi->itu=getZoneColor(TAMB-273.15,zonemin,zonemax,nrgb_full);
  roomi->rho_L=PAMB/R/roomi->tl;
  roomi->rho_U=PAMB/R/roomi->tu;
  roomi->z0=0.0;
  roomi->z1=100000.0;
}

/* ------------------ get_p ------------------------ */

float get_p(float y, float pfloor, float ylay, float rho_L, float rho_U){
  float g=9.80;
  float p;

  if(y<ylay){
    p = pfloor - rho_L*g*y*xyzmaxdiff;
  }
  else{
    p = pfloor - rho_L*g*ylay*xyzmaxdiff;
    p -= rho_U*g*(y-ylay)*xyzmaxdiff;
  }
  return p;
}

/* ------------------ get_dpT ------------------------ */

void get_dpT(float *yy, int n, roomdata *r1, roomdata *r2, float *delp, int *iT){
  float p1, p2;
  int itslab;
  int fsign;
  int i;
  float y;

  for(i=0;i<n;i++){

    y=yy[i];

    if(y<r1->z0||y<r2->z0||y>r1->z1||y>r2->z1){
      delp[i]=0.0;
      iT[i]=r1->itl;
    }

    p1=get_p(y,              r1->pfloor,r1->ylay,r1->rho_L,r1->rho_U);
    p2=get_p(y+r1->z0-r2->z0,r2->pfloor,r2->ylay,r2->rho_L,r2->rho_U);

    if(p1>p2){
      fsign=1.0;
      if(y>r1->ylay){
        itslab=r1->itu;
      }
      else{
        itslab=r1->itl;
      }
    }
    else{
      fsign=-1.0;
      if(y>r2->ylay){
        itslab=r2->itu;
      }
      else{
        itslab=r2->itl;
      }
    }
    delp[i]=fsign*sqrt(fabs(p1-p2));
    iT[i]=itslab;
  }
  return;
}

/* ------------------ drawroomgeom ------------------------ */

void drawroomgeom(void){
  float xroom0, yroom0, zroom0, xroom, yroom, zroom;
  float x1,x2,y1,y2,z1,z2;
  int i;
  int idir;
  float yy,zz;
  
  fill_roomdata(izone);

  if(cullfaces==1)glDisable(GL_CULL_FACE);


  if(use_transparency_data==1)transparenton();

/* draw the frame */

  antialias(1);
  glLineWidth(linewidth);
  glColor4fv(foregroundcolor);
  glBegin(GL_LINES);

  for(i=0;i<nrooms;i++){
    roomdata *roomi;

    roomi = roominfo + i;
    xroom0 = roomi->x0;
    yroom0 = roomi->y0;
    zroom0 = roomi->z0;
    xroom = roomi->x1;
    yroom = roomi->y1;
    zroom = roomi->z1;

 
    glVertex3f(xroom0,yroom0,zroom);
    glVertex3f(xroom,yroom0,zroom);

    glVertex3f(xroom,yroom0,zroom);
    glVertex3f(xroom,yroom,zroom);

    glVertex3f(xroom,yroom,zroom);
    glVertex3f(xroom0,yroom,zroom);

    glVertex3f(xroom0,yroom,zroom);
    glVertex3f(xroom0,yroom0,zroom);

    glVertex3f(xroom0,yroom0,zroom0);
    glVertex3f(xroom,yroom0,zroom0);

    glVertex3f(xroom,yroom0,zroom0);
    glVertex3f(xroom,yroom,zroom0);

    glVertex3f(xroom,yroom,zroom0);
    glVertex3f(xroom0,yroom,zroom0);

    glVertex3f(xroom0,yroom,zroom0);
    glVertex3f(xroom0,yroom0,zroom0);

    glVertex3f(xroom0,yroom0,zroom0);
    glVertex3f(xroom0,yroom0,zroom);

    glVertex3f(xroom,yroom0,zroom0);
    glVertex3f(xroom,yroom0,zroom);

    glVertex3f(xroom,yroom,zroom0);
    glVertex3f(xroom,yroom,zroom);

    glVertex3f(xroom0,yroom,zroom0);
    glVertex3f(xroom0,yroom,zroom);
  }
  glEnd();
  antialias(0);

  if(visVents==1){
    glLineWidth(ventlinewidth);
    for(i=0;i<nzvents;i++){
      zvent *zvi;

      zvi = zventinfo + i;

      glColor4fv(zvi->color);
      idir=zvi->dir;
      x1=zvi->x1;
      x2=zvi->x2;
      z1=zvi->z1;
      z2=zvi->z2;
      yy=zvi->yy;
      glBegin(GL_LINE_LOOP);
      switch (idir){
      case 1:
      case 3:
        glVertex3f(x1,yy,z1);
        glVertex3f(x2,yy,z1);
        glVertex3f(x2,yy,z2);
        glVertex3f(x1,yy,z2);
        glVertex3f(x1,yy,z1);
        break;
      case 2:
      case 4:
        glVertex3f(yy,x1,z1);
        glVertex3f(yy,x2,z1);
        glVertex3f(yy,x2,z2);
        glVertex3f(yy,x1,z2);
        glVertex3f(yy,x1,z1);
        break;
      case 5:
      case 6:
        y1=zvi->y1;
        y2=zvi->y2;
        zz=zvi->zz;
        glVertex3f(x1,y1,zz);
        glVertex3f(x2,y1,zz);
        glVertex3f(x2,y2,zz);
        glVertex3f(x1,y2,zz);
        glVertex3f(x1,y1,zz);
        break;
      default:
        ASSERT(FFALSE);
        break;
      }
      glEnd();
    }
  }

  if(use_transparency_data==1)transparentoff();
  if(cullfaces==1)glEnable(GL_CULL_FACE);
}

/* ------------------ drawventdata ------------------------ */

void drawventdata(void){
  float vmin, vmax;
  float factor;
  int i;
  int idir;
  float x1, yy;

  if(visVents==0)return;
  if(visVentLines==0&&visVentSolid==0)return;
  vmin=1000000000.0;
  vmax=-vmin;

  if(cullfaces==1)glDisable(GL_CULL_FACE);

  for(i=0;i<nzvents;i++){
    int j;
    zvent *zvi;
    float yelev[20];

    zvi = zventinfo + i;
    if(zvi->vent_orien==1)continue;
    for(j=0;j<20;j++){
      yelev[j]=(zvi->z1*(19-j)+zvi->z2*j)/19.0;
    }
    get_dpT(yelev, 20, zvi->room1, zvi->room2, zvi->vdata, zvi->itempdata);
    for(j=0;j<20;j++){
      if(zvi->vdata[j]>vmax)vmax=zvi->vdata[j];
      if(zvi->vdata[j]<vmin)vmin=zvi->vdata[j];
    }
  }
  if(vmin<0.0)vmin=-vmin;
  if(vmax<0.0)vmax=-vmax;
  if(vmin>vmax)vmax=vmin;
  factor=0.1/vmax;

  for(i=0;i<nzvents;i++){
    zvent *zvi;
    int j;
    float yelev[20];
    float *vcolor1,*vcolor2;

    zvi = zventinfo + i;

    if(zvi->vent_orien==1)continue;
    for(j=0;j<20;j++){
      yelev[j]=(zvi->z1*(19-j)+zvi->z2*j)/19.0;
    }
    idir=zvi->dir;
    x1=(zvi->x1+zvi->x2)/2.0;
    yy=zvi->yy;
    if(visVentSolid==1){
      glBegin(GL_QUADS);
      for(j=0;j<19;j++){
        float dy1,dy2;

        dy1 = factor*zvi->vdata[j];
        dy2 = factor*zvi->vdata[j+1];
        if(idir==1||idir==4){
          dy1=-dy1;
          dy2=-dy2;
        }
        vcolor1=rgb_full[zvi->itempdata[j]];
        vcolor2=rgb_full[zvi->itempdata[j+1]];
        vcolor2=vcolor1;
        switch (idir){
        case 4:
        case 2:
          if(dy1*dy2>=0.0){
            glColor3fv(vcolor1);
            glVertex3f(yy,    x1,yelev[j]);
            glVertex3f(yy+dy1,x1,yelev[j]);
       
            glColor3fv(vcolor2);
            glVertex3f(yy+dy2,x1,yelev[j+1]);
            glVertex3f(yy,    x1,yelev[j+1]);
          }
          else{
            float dyy;

            dyy =  yelev[j] - dy1*(yelev[j+1]-yelev[j])/(dy2-dy1);
            glColor3fv(vcolor1);
            glVertex3f(yy,    x1,yelev[j]);
            glVertex3f(yy+dy1,x1,yelev[j]);
            glVertex3f(yy,    x1,dyy);
            glVertex3f(yy,x1,dyy);


            glColor3fv(vcolor2);
            glVertex3f(yy,    x1,dyy);
            glVertex3f(yy,    x1,dyy);
            glVertex3f(yy+dy2,x1,yelev[j+1]);
            glVertex3f(yy,    x1,yelev[j+1]);
          }
          break;
        case 3:
        case 1:
          if(dy1*dy2>=0.0){
            glColor3fv(vcolor1);
            glVertex3f(x1,yy,    yelev[j]);
            glVertex3f(x1,yy+dy1,yelev[j]);

            glColor3fv(vcolor2);
            glVertex3f(x1,yy+dy2,yelev[j+1]);
            glVertex3f(x1,yy,    yelev[j+1]);
          }
          else{
            float dyy;

            dyy =  yelev[j] - dy1*(yelev[j+1]-yelev[j])/(dy2-dy1);
            glColor3fv(vcolor1);
            glVertex3f(x1,yy,    yelev[j]);
            glVertex3f(x1,yy+dy1,yelev[j]);
            glVertex3f(x1,yy,dyy);
            glVertex3f(x1,yy,dyy);

            glColor3fv(vcolor2);
            glVertex3f(x1,yy,dyy);
            glVertex3f(x1,yy,dyy);
            glVertex3f(x1,yy+dy2,yelev[j+1]);
            glVertex3f(x1,yy,    yelev[j+1]);
          }
          break;
        default:
          ASSERT(FFALSE);
          break;
        }
      }
      glEnd();
    }
    if(visVentLines==1){
      glBegin(GL_LINES);
      for(j=0;j<20;j++){
        float dy1;

        dy1 = factor*zvi->vdata[j];
        vcolor1=rgb_full[zvi->itempdata[j]];
        glColor3fv(vcolor1);
        switch (idir){
        case 1:
          glVertex3f(x1,yy,    yelev[j]);
          glVertex3f(x1,yy-dy1,yelev[j]);
          break;
        case 2:
          glVertex3f(yy,    x1,yelev[j]);
          glVertex3f(yy+dy1,x1,yelev[j]);
          break;
        case 3:
          glVertex3f(x1,yy,    yelev[j]);
          glVertex3f(x1,yy+dy1,yelev[j]);
          break;
        case 4:
          glVertex3f(yy,    x1,yelev[j]);
          glVertex3f(yy-dy1,x1,yelev[j]);
          break;
        default:
          ASSERT(FFALSE);
          break;
        }
      }
      glEnd();
    }
  }
  if(cullfaces==1)glEnable(GL_CULL_FACE);

}

/* ------------------ drawzonesmoke ------------------------ */

void drawzonesmoke(roomdata *roomi){
  float xroom0, yroom0, zroom0, xroom, yroom, zroom;
  float ylay,dy;

  float color4[4]={1.0,0.0,0.0,1.0};

  xroom0 = roomi->x0;
  yroom0 = roomi->y0;
  xroom = roomi->x1;
  yroom = roomi->y1;
  zroom0 = roomi->z0;
  zroom = roomi->z1;
  ylay = roomi->ylay;
  dy = roomi->dy/2.;
  glBegin(GL_QUADS);
  glColor4f(1.0,0.0,0.0,1.0);
  glVertex3f(xroom0,yroom0+dy,ylay+zroom0);
  glVertex3f(xroom,yroom0+dy,ylay+zroom0);
  glVertex3f(xroom, yroom0+dy,zroom);
  glVertex3f(xroom0,yroom0+dy,zroom);
  glColor4f(0.0,0.0,1.0,1.0);
  glVertex3f(xroom0, yroom0+dy,zroom0);
  glVertex3f(xroom,yroom0+dy,zroom0);
  glVertex3f(xroom,yroom0+dy,ylay+zroom0);
  glVertex3f(xroom0,yroom0+dy,ylay+zroom0);
  glEnd();
}

/* ------------------ drawroomdata ------------------------ */

void drawroomdata(void){
  float xroom0, yroom0, zroom0, xroom, yroom, zroom;
  float *zoneylaybase,dy;
  unsigned char *hazardcolorbase, *zonecolorbase;
  float *zoneqfirebase;
  float ylay;
  float qdot;
  float radius, height;
  float *colorv;
  unsigned char color;
  unsigned char *izonetubase;
  int i;

  if(zonet[0]>times[itimes])return;
  if(cullfaces==1)glDisable(GL_CULL_FACE);


  if(use_transparency_data==1)transparenton();

  izonetubase = izonetu + izone*nrooms;
  hazardcolorbase = hazardcolor + izone*nrooms;
  zoneylaybase = zoneylay + izone*nrooms;
  zoneqfirebase = zoneqfire + izone*nfires;

  if(sethazardcolor==1){
    zonecolorbase=hazardcolorbase;
  }
  else{
    zonecolorbase=izonetubase;
  }

  for(i=0;i<nrooms;i++){
    roomdata *roomi;

    roomi = roominfo + i;

    ylay = *(zoneylaybase+i);
    color = *(zonecolorbase+i);
    if(sethazardcolor==1){
      colorv = rgbhazard[color];
    }
    else{
      colorv = rgb_full[color];
    }
    xroom0 = roomi->x0;
    yroom0 = roomi->y0;
    xroom = roomi->x1;
    yroom = roomi->y1;
    zroom0 = roomi->z0;
    zroom = roomi->z1;
    dy = roomi->dy/2.;

    if(visODZone==1&&zoneodl!=NULL&&zoneodu!=NULL){
      drawzonesmoke(roomi);
    }
    else{
      if(visHZone==1){
        glColor4fv(colorv);
        glBegin(GL_QUADS);
        glVertex3f(xroom0,yroom0,ylay+zroom0);
        glVertex3f(xroom,yroom0,ylay+zroom0);
        glVertex3f(xroom,yroom,ylay+zroom0);
        glVertex3f(xroom0,yroom,ylay+zroom0);
        glEnd();
      }
      if(visVZone==1){
        glColor4fv(colorv);
        glBegin(GL_QUADS);
        glVertex3f(xroom0,yroom0+dy,ylay+zroom0);
        glVertex3f(xroom,yroom0+dy,ylay+zroom0);
        glVertex3f(xroom, yroom0+dy,zroom);
        glVertex3f(xroom0,yroom0+dy,zroom);
        glEnd();
      }
    }
  }

  if(viszonefire==1){
    glColor3f(0.7f,0.7f,0.7f);
    for(i=0;i<nfires;i++){
      qdot = zoneqfirebase[i]/1000.0f;
      if(qdot>0.0f){
        firedata *firei;

        firei = fireinfo + i;
        height = (0.23f*pow((double)qdot,(double)0.4)/(1.0f+2.0f*0.268f))/xyzmaxdiff;
        radius = height*0.268f;
        glPushMatrix();
        glTranslatef(firei->absx,firei->absy,firei->absz);
        DrawCone(radius,height);
        glPopMatrix();
      }
    }
  }
  if(use_transparency_data==1)transparentoff();
  if(cullfaces==1)glEnable(GL_CULL_FACE);
}

/* ------------------ DrawCone ------------------------ */

void DrawCone(float radius, float height){
#define NX 13
#define NH 9
  int i,j;
  float x[NX]={1.,.866,.500,.000,-.500, -.866,-1.0,-.866,-.500,  0., .5,   .866,1.};
  float y[NX]={0.,.500,.866,1.00, .866,  .500, 0.0,-.500,-.866,-1.,-.866,-.5,  0.};
  float h[NH]={1.,.875,.750,.625, .500,  .375, .25, .125, .000};

  glPushMatrix();
  glScalef(radius,radius,height);
  for(i=0;i<NX-1;i++){
    glBegin(GL_QUAD_STRIP);
    for(j=0;j<NH-1;j++){
      glVertex3f(h[j]*x[i],h[j]*y[i],h[j]);
      glVertex3f(h[j]*x[i+1],h[j]*y[i+1],h[j]);
    }
    glEnd();
  }
  glPopMatrix();
}

