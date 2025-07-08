messageExpression
    : '[' receiver messageSelector ']'
    ;

receiver
    : expression
    | typeSpecifier
    ;


selector
    : identifier
    | 'return'
    ;

messageSelector
    : selector
    | keywordArgument+
    ;

keywordArgument
    : selector? ':' keywordArgumentType (',' keywordArgumentType)*
    ;

keywordArgumentType
    : expression ('{' initializerList '}')?
    ;

selectorExpression
    : '@selector' LP selectorName RP
    ;

selectorName
    : selector
    | UNDERSCORE
    | (selector? ':')+
    ;

encodeExpression
    : '@encode' LP typeName RP
    ;


structOrUnionSpecifier
    : ('struct' | 'union') (identifier | identifier? '{' fieldDeclaration+ '}')
    ;

fieldDeclaration
    : typeSpecifier fieldDeclaratorList macro? ';'
    ;


typeQualifier
    : CONST
    | VOLATILE
    | RESTRICT
    ;

typeSpecifier
    : 'void' typeQualifier*
    | numericSignModifier? 'char' typeQualifier*
    | numericSignModifier? 'short' typeQualifier*
    | numericSignModifier? ('short' | 'long' | 'long' 'long')? 'int' typeQualifier*
    | numericSignModifier? 'long'? 'long' typeQualifier*
    | 'float' typeQualifier*
    | 'long'? 'double' typeQualifier*
    | structOrUnionSpecifier
    | enumSpecifier
    | 'id' typeQualifier*
    | identifier typeQualifier*
    | typeSpecifier '*' typeQualifier*
    ;


fieldDeclaratorList
    : fieldDeclarator (',' fieldDeclarator)*
    ;

fieldDeclarator
    : declarator
    | declarator? ':' constant
    ;

enumSpecifier
    : 'enum' (identifier? ':' typeName)? (
        identifier ('{' enumeratorList '}')?
        | '{' enumeratorList '}'
    )
    ;

enumeratorList
    : enumerator (',' enumerator)* ','?
    ;

enumerator
    : enumeratorIdentifier ('=' assignmentExpression)?
    ;

enumeratorIdentifier
    : identifier
    | 'default'
    ;

declarator
    : (identifier | LP declarator RP) declaratorSuffix*
    ;

declaratorSuffix
    : '[' constantExpression? ']'
    ;

parameterList
    : parameterDeclarationList (',' '...')?
    ;


arrayInitializer
    : '{' (expression ','?)? '}'
    ;

// Designated struct initializer
// e.g. struct point_t a = { .y = 2, .x = 1 };
structInitializer
    : '{' ('.' structAssignmentExpression (',' '.' structAssignmentExpression)* ','?)? '}'
    ;
    
structAssignmentExpression
    : identifier '=' assignmentExpression
    ;
    
initializerList
    : initializer (',' initializer)* ','?
    ;

typeName
    : typeSpecifier abstractDeclarator?
    ;

abstractDeclarator
    : LP abstractDeclarator? RP abstractDeclaratorSuffix+
    | ('[' constantExpression? ']')+
    ;

abstractDeclaratorSuffix
    : '[' constantExpression? ']'
    | LP parameterDeclarationList? RP
    ;

parameterDeclarationList
    : parameterDeclaration (',' parameterDeclaration)*
    ;

parameterDeclaration
    : typeSpecifier declarator
    | 'void'
    ;

castExpression
    : (LP typeName RP) castExpression
    | unaryExpression
    ;

multiplicativeExpression
    : castExpression (('*' | '/' | '%') castExpression)*
    ;

additiveExpression
    : multiplicativeExpression (('+' | '-') multiplicativeExpression)*
    ;

shiftExpression
    : additiveExpression ((leftShiftOperator | rightShiftOperator) additiveExpression)*
    ;
    
leftShiftOperator
    : LT LT
    ;
    
rightShiftOperator
    : GT GT
    ;

relationalExpression
    : shiftExpression ((LT | GT | LE | GE) shiftExpression)*
    ;

equalityExpression
    : relationalExpression ((EQUAL | NOTEQUAL) relationalExpression)*
    ;

andExpression
    : equalityExpression (BITAND equalityExpression)*
    ;

exclusiveOrExpression
    : andExpression (BITXOR andExpression)*
    ;

inclusiveOrExpression
    : exclusiveOrExpression (BITOR exclusiveOrExpression)*
    ;

logicalAndExpression
    : inclusiveOrExpression (AND inclusiveOrExpression)*
    ;

logicalOrExpression
    : logicalAndExpression (OR logicalAndExpression)*
    ;
    
conditionalExpression
    : logicalOrExpression ('?' ifExpr = conditionalExpression? ':' elseExpr = conditionalExpression)?
    ;
    
assignmentExpression
    : conditionalExpression
    | unaryExpression assignmentOperator assignmentExpression
    ;
    
assignmentOperator
    : '='
    | '*='
    | '/='
    | '%='
    | '+='
    | '-='
    | '<<='
    | '>>='
    | '&='
    | '^='
    | '|='
    ;
    
expression
    : assignmentExpression (',' assignmentExpression)*
    ;

constantExpression
    : conditionalExpression
    ;
    
initializer
    : assignmentExpression
    | arrayInitializer
    | structInitializer
    ;

// The expression that is allowed on the left-hand-side of the assignment operator
unaryExpression
    : ('++' | '--')* (
        postfixExpression
        | unaryOperator castExpression
        | ('sizeof' | '_Alignof') LP typeName RP
        | AND identifier // GCC extension address of label
    )
    ;

unaryOperator
    : '&'
    | '*'
    | '+'
    | '-'
    | '~'
    | BANG
    ;

postfixExpression
    : (primaryExpression | '__extension__'? LP typeName RP '{' initializerList ','? '}') (
        '[' expression ']'
        | LP argumentExpressionList? RP
        | ('.' | '->') identifier
        | '++'
        | '--'
    )*
    ;

postfix
    : LBRACK expression RBRACK
    | LP argumentExpressionList? RP
    | LP (COMMA | macroArguments += ~RP)+ RP
    | op = (INC | DEC)
    ;

argumentExpressionList
    : argumentExpression (',' argumentExpression)*
    ;

argumentExpression
    : expression
    | typeSpecifier
    ;

primaryExpression
    : identifier
    | constant
    | stringLiteral
    | LP expression RP
    | messageExpression
    | encodeExpression
    ;

constant
    : HEX_LITERAL
    | OCTAL_LITERAL
    | BINARY_LITERAL
    | ('+' | '-')? DECIMAL_LITERAL
    | ('+' | '-')? FLOATING_POINT_LITERAL
    | CHARACTER_LITERAL
    | NIL
    | NULL_
    | YES
    | NO
    | TRUE
    | FALSE
    ;

stringLiteral
    : (STRING_START (STRING_VALUE | STRING_NEWLINE)* STRING_END)+
    ;

identifier
    : IDENTIFIER
    | BOOL
    | Class
    | ID
    | IMP
    | SEL
    | SELF
    | SUPER
    ;
