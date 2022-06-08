#include <stdio.h>
#include <glib.h>

int main(int argc, char **argv)
{

  --argc;
  ++argv;

  if (argc != 2){
    printf("usage: ./ex_tspl <dev> <model>\n");
    exit(1);
  }
  //fd = open(argv[0], O_RDWR);
  //if (fd < 0)
  //  error (1, "Can't open '%s'\n", argv[0]);

  int dots=8;
  int mgr=0;

  if (strcmp(argv[1], "P200") == 0)
    mgr = 54; // 7mm*8dots

  char *buff;
  char *ticketno = "A1234567890"; //1
  char *route = "933 กทม-ศชม.";
  char *name = "ชิตมนัส ชินารมย์";
  char *booking = "พลอยไพรินทร์";

  char *depdate = "20/12/2022"; //2
  char *fare = "9999";
  char *issuedate = "10/12/2022";

  char *deptime = "20:30"; //3
  char *extra = "0";
  char *discount = "3333";
  char *tripno = "555";

  char *seatno = "14D";  //4
  char *total = "9999";
  char *platform = "888";

  buff = g_strconcat(
           "SIZE 3,8\n",
           "BLINE 4mm, 2mm\n",
           "OFFSET 4mm\n",
           "DIRECTION 0\n",
           "DENSITY 8\n",
           "CODEPAGE UTF-8\n",
           "CLS\n",
           "QRCODE ",g_strdup_printf("%i", 68*dots),", ",g_strdup_printf("%i", 80*dots+mgr),", ",
                    "H",", ", "4",", ", "A", ", ", "90", ", ", "M2", ", \"",ticketno,"\"\n",
           "TEXT ", g_strdup_printf("%i", 68*dots),", ",g_strdup_printf("%i", 195*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 3, \"",tripno, "-", seatno,"\"\n",
           "TEXT ", g_strdup_printf("%i", 60*dots),", ",g_strdup_printf("%i", 7*dots+mgr),", ", //1
                    "\"LOMAR.TTF\", 90, 12, 12, 1, \"",ticketno,"\"\n",
           "TEXT ", g_strdup_printf("%i", 50*dots),", ",g_strdup_printf("%i", 5*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 1, \"",route,"\"\n",
           "TEXT ", g_strdup_printf("%i", 36*dots),", ",g_strdup_printf("%i", 5*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 1, \"",name,"\"\n",
           "TEXT ", g_strdup_printf("%i", 22*dots),", ",g_strdup_printf("%i", 3*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 10, 10, 1, \"",booking,"\"\n",
           "TEXT ", g_strdup_printf("%i", 50*dots),", ",g_strdup_printf("%i", 33*dots+mgr),", ", //2
                    "\"LOMAR.TTF\", 90, 16, 16, 1, \"",depdate,"\"\n",
           "TEXT ", g_strdup_printf("%i", 36*dots),", ",g_strdup_printf("%i", 38*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 1, \"",fare,"\"\n",
           "TEXT ", g_strdup_printf("%i", 22*dots),", ",g_strdup_printf("%i", 45*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 2, \"",issuedate,"\"\n",
           "TEXT ", g_strdup_printf("%i", 50*dots),", ",g_strdup_printf("%i", 66*dots+mgr),", ", //3
                    "\"LOMAR.TTF\", 90, 16, 16, 1, \"",deptime,"\"\n",
           "TEXT ", g_strdup_printf("%i", 36*dots),", ",g_strdup_printf("%i", 65*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 2, \"",extra,"\"\n",
           "TEXT ", g_strdup_printf("%i", 36*dots),", ",g_strdup_printf("%i", 80*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 1, \"",discount,"\"\n",
           "TEXT ", g_strdup_printf("%i", 22*dots),", ",g_strdup_printf("%i", 75*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 2, \"",tripno,"\"\n",
           "TEXT ", g_strdup_printf("%i", 50*dots),", ",g_strdup_printf("%i", 95*dots+mgr),", ", //4
                    "\"LOMAR.TTF\", 90, 16, 14, 1, \"",seatno,"\"\n",
           "TEXT ", g_strdup_printf("%i", 36*dots),", ",g_strdup_printf("%i", 97*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 1, \"",total,"\"\n",
           "TEXT ", g_strdup_printf("%i", 22*dots),", ",g_strdup_printf("%i", 100*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 2, \"",platform,"\"\n",
           "TEXT ", g_strdup_printf("%i", 60*dots),", ",g_strdup_printf("%i", 132*dots+mgr),", ", //5-Align:center
                    "\"LOMAR.TTF\", 90, 12, 12, 2, \"",ticketno,"\"\n",
           "TEXT ", g_strdup_printf("%i", 50*dots),", ",g_strdup_printf("%i", 132*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 2, \"",route,"\"\n",
           "TEXT ", g_strdup_printf("%i", 36*dots),", ",g_strdup_printf("%i", 132*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 2, \"",tripno,"-",seatno,"\"\n",
           "TEXT ", g_strdup_printf("%i", 22*dots),", ",g_strdup_printf("%i", 132*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 2, \"",depdate,"\"\n",
           "TEXT ", g_strdup_printf("%i", 60*dots),", ",g_strdup_printf("%i", 160*dots+mgr),", ", //6
                    "\"LOMAR.TTF\", 90, 12, 12, 1, \"",ticketno,"\"\n",
           "TEXT ", g_strdup_printf("%i", 50*dots),", ",g_strdup_printf("%i", 153*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 10, 10, 1, \"933\"\n",
           "TEXT ", g_strdup_printf("%i", 50*dots),", ",g_strdup_printf("%i", 163*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 1, \"",depdate,"\"\n",
           "TEXT ", g_strdup_printf("%i", 36*dots),", ",g_strdup_printf("%i", 153*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 10, 10, 1, \"",name,"\"\n",
           "TEXT ", g_strdup_printf("%i", 22*dots),", ",g_strdup_printf("%i", 153*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 10, 10, 1, \"",booking,"\"\n",
           "TEXT ", g_strdup_printf("%i", 50*dots),", ",g_strdup_printf("%i", 186*dots+mgr),", ", //7
                    "\"LOMAR.TTF\", 90, 12, 12, 1, \"",deptime,"\"\n",
           "TEXT ", g_strdup_printf("%i", 36*dots),", ",g_strdup_printf("%i", 188*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 1, \"",total,"\"\n",
           "TEXT ", g_strdup_printf("%i", 22*dots),", ",g_strdup_printf("%i", 187*dots+mgr),", ",
                    "\"LOMAR.TTF\", 90, 12, 12, 2, \"",issuedate,"\"\n",
           "PRINT 1\n",
           NULL);

  printf("%s\n", buff);
  g_free(buff);

  return 0;
}
