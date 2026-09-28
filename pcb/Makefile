PROJECT=starman
BOM_CSV=../bom.csv
SCH_PDF=../schematic.pdf
PCB_PDF=../pcb.pdf

all: ${BOM_CSV} ${SCH_PDF} #${PCB_PDF}

${BOM_CSV}: ${PROJECT}.kicad_sch
	kicad-cli sch export bom $^ -o $@ --group-by Value --fields "Reference,Value,Footprint,Description,Quantity" --ref-delimiter ", "

${SCH_PDF}: ${PROJECT}.kicad_sch
	kicad-cli sch export pdf $^ -o $@

${PCB_PDF}: ${PROJECT}.kicad_pcb
	kicad-cli pcb export pdf $^ -o $@ -l B.Silkscreen,F.Cu,B.Cu,In2.Cu,F.Silkscreen,F.Mask,F.Adhesive,F.Paste,F.Courtyard,Edge.Cuts
