#include<iostream>
#include<string.h>

using namespace std;

class Block{
    public:
    int operator_value; //The bigger, the more priority an operator is. Number and Alpha blocks have 0 operator_value
    string content;     //Name of the block, can be +, -. *... or 17, 489, A, B...
    Block* next;        

    int flag;           //When switching position, the relevant blocks will be updated with new flag.
                        //When one block is being switching position, the whole bunch of blocks with same flag will move with it.

    Block(int o_v,string c){
        operator_value=o_v;
        content=c;
        next=NULL;

        flag=0;
    }
};

class List{
    public:
    Block* head;
    Block* tail;

    int flag;

    List(){
        Block* B=new Block(0,"");
        head=B;
        tail=B;
        flag=1000;
    }
    void push(int o_v, string c){
        Block* B=new Block(o_v,c);

        tail->next=B;
        tail=B;
    }

    //SWITCH function:
    //                                                               THIS   THAT
    //                                                                  ↓ ↓ ↓
    //Switch will locate an operator and its relative left and right ...A * B..., then switch to ...* A B...
    //After switching, ...[* A B]... will always stick together, if *, A or B is the relative left and right of other switching process,
    //that process will switch whole bunch of [* A B].  
    //                                             Ex:  ...[* A B] + 45...   --switch->   ...[+ * A B 45]... (these five blocks now have
    //                                                          ↑  ↑ ↑                                        the same new flag value)
    //We stick them by giving them the same flag value.      THIS    THAT

    void SWITCH(Block* THIS, Block* OP, Block* THAT){
        int THATflag=THAT->flag;

        Block* decoy=head;
        Block* p; //Runs to every blocks need to update the flag

        //SWITCH THEN UPDATE FLAG OF [OP] and [THIS]
        if(THIS->flag==0)
        {
            while(decoy!=NULL)
            {
                if(decoy->next==THIS)
                {
                    OP->flag=this->flag;
                    THIS->flag=this->flag;

                    decoy->next=OP;
                    OP->next=THIS;
                    THIS->next=THAT;

                    break;
                }
                decoy=decoy->next;
            }
        }
        else
        {
            while(decoy!=NULL)
            {
                if(decoy->next->flag==THIS->flag)
                {
                    p=decoy->next;
                    while(p->flag==THIS->flag)
                    {
                        p->flag=this->flag;
                        p=p->next;
                    }
                    OP->flag=this->flag;

                    OP->next=decoy->next;
                    decoy->next=OP;
                    THIS->next=THAT;

                    break;
                }
                decoy=decoy->next;
            }
        }

        //UPDATE FLAG OF [THAT]
        if(THAT->flag==0)
        {
            THAT->flag=this->flag;
        }
        else
        {
            p=THAT;
            while(p->flag==THATflag)
            {
                p->flag=this->flag;
                p=p->next;

                if(p==NULL) break;
            }
        }

        flag+=1000;

        //for checking
        //display();
    }

    void display(){
        cout<<"\n--------------\n";
        for(Block* decoy=head->next;decoy!=NULL;decoy=decoy->next)
        {
            cout<<decoy->content<<" "<<decoy->flag<<"\n\n";
        }
        cout<<"\n--------------\n";
    }
};

bool isAlpha(char c){
    if((c>=65 && c<=90) || (c>=97 && c<=122))
        return true;
    else
        return false;
}
bool isDigit(char c){
    if(c>=48 && c<=57)
        return true;
    else
        return false;
}
bool isOp(char c){
    if(c==43 || c==45 || c==42 || c==47 || c==94)
        return true;
    else
        return false;
}
void PrefixToPostfix(List &MyList, int NumberOfOperators){
    //Prefix list to Postfix list
    /*An operator will switch to the next position where its [THAT] ends.

/    ...+ A B...              --change-->     ...A B +...
/           ↑                                  
/           [THAT] END

/    ...+ [* A B] [^ C D]     --change-->     ...[* A B] [^ C D] +...
/                      ↑                                  
/                      [THAT] END

    How to find THAT END?
/                                                      (+)                        (A)      (B)
    Observed above, we can see that a simple operator [OP] have 2 binding values THIS and THAT
    When [THIS] or [THAT] is an another operator, number of binding values to [OP] increase by 1.
    -> Initialize [OP] have /int desc=2/ binding values, and keep going right,
/         if meet its binding value, desc--
/         if meet another operator, desc++
/   -> When desc=0, [OP] has gone to the end of [THAT]
/
/   Ex:    Pre: + - c 7 * 8 - ^ 7 P / 6 - 5 d
/               ↑ 
/            [OP]    
/               ↑ 
/     int desc=2 3 2 1 2 1 2 3 2 1 2 1 2 1 0
/                                           ↑ 
                                            [THAT] END
    */

    while(NumberOfOperators>0)
    {
        for(Block* decoy=MyList.head;decoy->next!=NULL;decoy=decoy->next)
        {
            if(decoy->next->operator_value==-1)
            {
                decoy->next->operator_value=-2;
                int desc=2;
                Block* OP=decoy->next;
               
                for(Block* p=decoy->next->next;p!=NULL;p=p->next)
                {
                    if(p->operator_value==-1)
                        desc++;
                    else
                        desc--;

                    if(desc==0)
                    {

                        decoy->next=decoy->next->next;
                        OP->next=p->next;
                        p->next=OP;

                        break;
                    }
                }

                break;
            }
        }
        NumberOfOperators--;
    }
}

//This function check syntax error, and also change the input string to the string that is easier to work with. Return "?" if syntax error.
string Check(string INPUT){
    int openBR=0;
    int closeBR=0;

    bool Dot;

    if(INPUT[0]=='.') return "?";

    //Check if final is an operator
    if(isOp(INPUT[INPUT.size()-1])) return "?";

    //Make continuous ...+-+-... become just + or -
    for(int i=0;i<INPUT.size()-1;i++)
    {
        if(INPUT[i]=='+' && INPUT[i+1]=='+') {INPUT[i]=' ';INPUT[i+1]='+';}
        if(INPUT[i]=='-' && INPUT[i+1]=='-') {INPUT[i]=' ';INPUT[i+1]='+';}
        if(INPUT[i]=='-' && INPUT[i+1]=='+') {INPUT[i]=' ';INPUT[i+1]='-';}
        if(INPUT[i]=='+' && INPUT[i+1]=='-') {INPUT[i]=' ';INPUT[i+1]='-';}

        if( (!isDigit(INPUT[i]) && INPUT[i+1]=='.') || (INPUT[i]=='.' && !isDigit(INPUT[i+1])) ) return "?";
        if(INPUT[i]=='.' && INPUT[i+1]=='.') return "?";

        //If this part is a number, check if there's many dot in there
        if(i>0 && !isDigit(INPUT[i-1]) && isDigit(INPUT[i]))
        {
            Dot=false;
            for(int j=i;j<INPUT.size() && (isDigit(INPUT[j]) || INPUT[j]=='.');j++)
            {
                if(Dot && INPUT[j]=='.') return "?";
                if(INPUT[j]=='.') Dot=true;
            }
        }
    }

    //Erase all blanks left from solving +-+- 
    string::iterator it=INPUT.begin();
    for(int i=0;i<INPUT.size();i++)
    {
        if(INPUT[i]==' ')
            INPUT.erase(it);
        it++;
    } 

    if(INPUT[0]=='+') INPUT.erase(INPUT.begin());
    if(INPUT[0]=='-') INPUT[0]='@';

    //Wrong syntax
    for(int i=0;i<INPUT.size();i++)
    {
        /*  Just    a->z or A->Z
                    0->9 or .
                    + - * / ^ ( )                         */
        if(!(isAlpha(INPUT[i]) || isDigit(INPUT[i]) || isOp(INPUT[i]) || INPUT[i]=='.' || INPUT[i]=='(' || INPUT[i]==')' || INPUT[i]=='@')) return "?";

        if(INPUT[i]=='(') openBR++;
        if(INPUT[i]==')') closeBR++;

        if(closeBR>openBR) return "?";

        if(i<INPUT.size()-1)
        {
            if((INPUT[i]=='*' || INPUT[i]=='/' || INPUT[i]=='^' || INPUT[i]=='-' || INPUT[i]=='+') && (INPUT[i+1]=='*' || INPUT[i+1]=='/' || INPUT[i+1]=='^')) return "?";

            //@ means minus
            if((INPUT[i]=='*' || INPUT[i]=='/' || INPUT[i]=='^' || INPUT[i]==' ' || INPUT[i]=='(') && (INPUT[i+1]=='-')) INPUT[i+1]='@';
            if((INPUT[i]=='*' || INPUT[i]=='/' || INPUT[i]=='^') && (INPUT[i+1]=='+')) INPUT[i+1]=' ';

            //Continuos Alpha and Digit
            if( (isAlpha(INPUT[i]) && isAlpha(INPUT[i+1])) || (isAlpha(INPUT[i]) && isDigit(INPUT[i+1])) || (isDigit(INPUT[i]) && isAlpha(INPUT[i+1])) ) return "?";
        }
    }

    //Invalid brackets
    if(openBR != closeBR) return "?";

    //Make A@B become A-B (but not .../@B... or ...*@B..., this will be solve in the creating Block process)
    for(int i=0;i<INPUT.size()-2;i++)
    {
        if( !(INPUT[i]=='*' || INPUT[i]=='/' || INPUT[i]=='^' || INPUT[i]=='(') && INPUT[i+1]=='@' && !(INPUT[i+2]=='*' || INPUT[i+2]=='/' || INPUT[i+2]=='^') )
            INPUT[i+1]='-';
    }

    return INPUT;
}

string InfixTo(string INPUT, string mode){
    if(mode!="prefix" && mode!="postfix") 
    {
        cout<<"\n! Wrong mode !\n";
        return INPUT;
    }

    INPUT=Check(INPUT);

    //for checking
    //cout<<"After check: "<<INPUT<<endl;

    if(INPUT=="?") return "Syntax error";

    string OUTPUT;
    int brackets_value=0;             //higher when inside lots of brackets
    int lefter_value=INPUT.size();    //lower when comes to the right of the string
    int counter=0;
    int NumberOfOperators=0;
    bool Dot;

    List MyList;

    for(int i=0;i<INPUT.size();i++)
    {
        if(INPUT[i]=='(')
            brackets_value+=10000;
        if(INPUT[i]==')')
            brackets_value-=10000;
        if(isAlpha(INPUT[i]))
        {
            string Alpha="";

            if(INPUT[i-1]=='@') Alpha+="-";

            Alpha+=INPUT[i];
            counter++;
            MyList.push(0,Alpha);
        }
        if(isDigit(INPUT[i]))
        {
            string Number="";

            if(INPUT[i-1]=='@') Number+="-";

            while(isDigit(INPUT[i]) || INPUT[i]=='.')
            {
                Number+=INPUT[i];
                i++;
            }
            i--;
            counter++;
            MyList.push(0,Number);
        }
        if(isOp(INPUT[i]))
        {
            int operator_value=brackets_value+lefter_value;
            switch(INPUT[i])
            {
                case '+':
                {
                    MyList.push(operator_value+1000,"+");
                } break;
                case '-':
                {
                    MyList.push(operator_value+1000,"-");
                } break;
                case '*':
                {
                    MyList.push(operator_value+2000,"*");
                } break;
                case '/':
                {
                    MyList.push(operator_value+2000,"/");
                } break;
                case '^':
                {
                    MyList.push(operator_value+3000,"^");
                } break;
            }
            NumberOfOperators++;
            counter++;
        }
        lefter_value--;
    }

    if(counter-2*NumberOfOperators!=1) return "Syntax errorr";

    int max;
    while(true)
    {
        max=0;
        Block* decoy=MyList.head;
        while(decoy!=NULL)
        {
            if(decoy->operator_value>max)
            {
                max=decoy->operator_value;
            }
            decoy=decoy->next;
        }

        //no more operator
        if(max==0) break;

        Block* THIS=MyList.head;
        Block* THAT;
        decoy=MyList.head;

        while(decoy->operator_value!=max)
        {
            THIS=THIS->next;

            decoy=THIS->next;
            THAT=decoy->next;
        }
        decoy->operator_value=-1;
        MyList.SWITCH(THIS,decoy,THAT);
    }

    if(mode=="postfix") PrefixToPostfix(MyList,NumberOfOperators);
    
    for(Block* decoy=MyList.head->next;decoy!=NULL;decoy=decoy->next)
    {
        OUTPUT+=decoy->content+" ";
    }

    return OUTPUT;
} 

int main(){
    cout<<"\nDUC 2152528 4thMay2022\n";

    string INPUT;
    string s;

    for(int i=0;i<10;i++)
    {
        cout<<"\nGive me the string: "; cin>>INPUT;
        cout<<"\nTransform to 'prefix' or 'postfix' or 'both': "; 

        AGAIN:
        cin>>s;
        if(s!="prefix" && s!="postfix" && s!="both")
        {
            cout<<"!! Just type 'prefix' or 'postfix' or 'both': ";
            goto AGAIN;
        }

        if(s=="prefix")
        {
            cout<<"\nPrefix: "<<InfixTo(INPUT,"prefix");
        }
        if(s=="postfix")
        {
            cout<<"\nPostfix: "<<InfixTo(INPUT,"postfix");
        }
        if(s=="both")
        {
            cout<<"\nPrefix:  "<<InfixTo(INPUT,"prefix");
            cout<<"\nPostfix: "<<InfixTo(INPUT,"postfix");
        }

        cout<<"\n-- DONE --\n";
    }

    return 0;
}
