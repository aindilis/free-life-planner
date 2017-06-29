$(document).ready(function() {
    var languageChecked = false;

    function filterNaughtyWords(txt,separator) {
        var nw = ['abortion','abuse','adult','alcoholic','amputation','anal','analingus','anilingus','anus','arousal','arse','aspermia','ass','asshole','assfucker','autoerotic','ballsack','autoeroticism','barenaked ladies','bareback','bastard','bestiality','bedwetting','bimbo','bisexual','bisexuality','bitch','bitches','bladder','blow job','blowjob','bodily','body','bollock','bollok','boner','boob','booob','breast','bowel','buggery','bugger','butt','buttplug','butthole','cadaver','carnal','catheter','cervical','cervicitis','cervix','cesarean','childbirth','circumcision','circumcise','circumcised','clit','clitoris','cock','cocksucker','cock-sucker','cox','cumdump','carpet muncher','clusterfuck','coitus','colonoscopy','colon','colorectal','colostomy','colostrum','condom','constipation','contraception','contraceptive','copulate','cornhole','cum','cumshot','cunilingus',
'cunillingus','cunt','cuntlicker','cyberfuck','cyberfucker','damn','decapitation','defecate','defecation','diaper','diarrhea','dick','dickhead','dildo','dildos','dirty sanchez','dimorphism','doggie','doggiestyle','douche','douchebag','drug','dyke','eclampsia','ecstasy','ectopic','egg','ejaculate','ejaculation','embryo','erectile','erection','erotic','eroticism','erotomania','excretion','excretory','exhibitionism','exhibitionist','facial','fanny','fag','faggot','faggit','faggitt','fcuk','fecal','feces','fellatio','female','fertility','fertilization','fibrocystic','fingerfuck','fingerfucker','fistfuck','fistfucker','felching','flatulence','flatulent','fuck','fucking','fuckhead','fucked','fuckme','fuck me','fucktoy','fucker','fudge packer','fudgepacker','gangbang','gangbanger','gangbanged','gaylord','gay','gaysex','gender','genitalia','genital','gestation','gestational diabetes','gonorrhea','group marriage','hardcore','hardcoresex','hell','hemorrhoid','hermaphrodite','hermaphroditism','hernia','herpesvirus','herpes','heterosexual','heterosexuality','homo','homoerotic','horny','hotsex','homosexual','homosexuality','husband-wife relations','hymen','hysterectomy','hysterosalpingography','hysteroscopy','implant','impregnate','incontinence','infant feeding','infertility','intercourse','intersex','intestine','intrauterine','jerkoff','jackoff','jack-off','jism','jizz','jock','jockitch','kinky','knob','knobjocky','knobhead','labia','labial','lactation','lesbian','lmfao','mammary','marijuana','masochism','masochist','mastectomy','mastitis','mastoidectomy','mastoiditis','masterbate','masterb8','masturbation','menopause','menstruation','miscarriage','monogamy','morning sickness','motherfucker','mother-fucker','mothafucker','mutilation','naked','narcotic','necrophilia','nigger','nigga','nob','nobhead','nobjockey','nutsack','nut butter','nipple','nudism','nude','nudity','oedipus','orgasm','ovarian','ovariectomy','ovary','ovaries','paedophilia','parasexual','pecker','phonesex','phuck','pedophile','pedophilia','pelvic','pelvis','penile','penis','perverse','pervert','perversion','pimp','piss','pissing','pms','poop','porn','pornography','pregnancy','premarital','premenstrual','prenatal','priapism','prolapse','promiscuous','promiscuity','prostate','prostatectomy','prostatic','prostatitis','prostitute','prostitution','psychosexual','pube','puberty','pussy','pussies','queef','prick','rimjaw','rimming','queer','rape','rectal','rectum','reproductive','reproduction','sadism','sadist','sadomasochism','sadomasochist','scrotum','seduction','self-mutilation','semen','schlong','screwing','screw','scrotum','shag','shagging','seminal','sexism','sexual','sexuality','sexually','sex','sexy','shit','shitdick','shitting','shithead','shitty','skank','snatch','smut','smegma','spunk','son-of-a-bitch','tosser','turd','twat','sodomy','spermatogenesis','spermatogonium','spermatozoon','sperm','sphincter','std','stool','stretch marks','suppository','tampon','testes','testicle','testicular','tit','tits','titties','the pill','toxic shock','transsexual','transvaginal','transvestism','transvestite','umbilical','umbilicus','unisexual','ureter','ureteral','ureterostomy','urethra','urethritis','urinalysis','urinary','urine','urogenital','urography','urological','urology','uterine','uterus','vagina','vaginitis','vagotomy','viagra','vas deferens','venereal','virginity','virgin','voyeurism','vulvar','vulva','vulvodynia','vulvovaginitis','yeast infection','wang','wank','wanker','whore','willy','willies','xxx','x-rated'];

        //+word1+word2+word3+
        separator = separator || '+';
        txt = separator + txt.toLowerCase().replace(/ |%20|%2b/g,separator) + separator; 
        nw.forEach(function(entry){
            //txt = txt.replace(new RegExp(separator + entry + separator,'g'), separator);
            txt = txt.replace(separator + entry + separator, separator);
            txt = txt.replace(separator + entry + 's' + separator, separator);
        });

        return txt.slice(1,txt.length-1);  //word1+word2+word3
    };
        
    function preventNaughtyWords(form) {
        languageChecked = true;
        var obj = form.find('.searchbox');
        obj.val(filterNaughtyWords(obj.val(),'+'));
        form.submit(); //submit the updated form
    };
    
    $('form').submit(function(e) {
        if(!languageChecked) {
            e.preventDefault(); //prevent form submission
            preventNaughtyWords($(this));
        }
    });

    $('.searchlink').click(function(e) {
        e.preventDefault();     //prevent form submission
        preventNaughtyWords($(this).closest('form'));    
    });
});
