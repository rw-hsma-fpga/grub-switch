    .section .text.bootfiles    ; subsequent subsection behind actual code in code flash
    .balign 2,0x00
    .global bootfile_template_start
    .global bootfile_template_end  
bootfile_template_start:
    .incbin "../bootfiles/template"
bootfile_template_end:
