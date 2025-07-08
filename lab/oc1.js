class ParseContext {
    constructor(input) {
        this.input = input;
        this.index = 0;
        this.length = input.length;
    }
}

function tryParse(parser, ctx, ...args) {
    const startIndex = ctx.index;
    const result = parser(ctx, ...args);
    if (result === null) ctx.index = startIndex;
    return result;
}

function skipWhitespace(ctx) {
    while (ctx.index < ctx.length && /\s/.test(ctx.input[ctx.index])) {
        ctx.index++;
    }
}

function token(ctx, regex) {
    skipWhitespace(ctx);
    const startIndex = ctx.index;
    const substr = ctx.input.substring(ctx.index);
    const match = substr.match(regex);
    if (match && match.index === 0) {
        ctx.index += match[0].length;
        return match[0];
    }
    return null;
}

// Token patterns
const TOKENS = {
    IDENTIFIER: /[a-zA-Z_][a-zA-Z0-9_]*/,
    HEX_LITERAL: /0[xX][0-9a-fA-F]+/,
    OCTAL_LITERAL: /0[0-7]+/,
    BINARY_LITERAL: /0[bB][01]+/,
    DECIMAL_LITERAL: /[1-9][0-9]*/,
    FLOATING_POINT_LITERAL: /[-+]?(\d+\.\d*|\.\d+|\d+)([eE][-+]?\d+)?[fFlL]?/,
    CHARACTER_LITERAL: /'(?:[^'\\]|\\.)*'/,
    STRING_LITERAL: /@"(?:[^"\\]|\\.)*"|"(?:[^"\\]|\\.)*"/,
  };

// Top-level expression parser
function parseExpression(ctx) {
    const exprs = [];
    let expr = tryParse(parseAssignmentExpression, ctx);
    if (!expr) return null;
    exprs.push(expr);
    while (tryParse(token, ctx, /,/)) {
        expr = tryParse(parseAssignmentExpression, ctx);
        if (!expr) break;
        exprs.push(expr);
    }
    scol = tryParse(token, ctx, /;/);
    if (scol)
        exprs.push(scol);
    return exprs.length === 1 ? exprs[0] : {type: 'expression', expressions: exprs};
}

function parseAssignmentExpression(ctx) {
    const start = ctx.index;
    const unary = tryParse(parseUnaryExpression, ctx);
    if (unary) {
        const op = tryParse(parseAssignmentOperator, ctx);
        if (op) {
            const right = tryParse(parseAssignmentExpression, ctx);
            if (right) return {type: 'assignment', left: unary, operator: op, right};
        }
    }
    ctx.index = start;
    return tryParse(parseConditionalExpression, ctx);
}

// Assignment operator parser
function parseAssignmentOperator(ctx) {
    const ops = ['=', '*=', '/=', '%=', '+=', '-=', '<<=', '>>=', '&=', '^=', '|='];
    for (const op of ops) {
        if (tryParse(token, ctx, new RegExp(op.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))) return op;
    }
    return null;
}

// Conditional expression parser
function parseConditionalExpression(ctx) {
    const expr = tryParse(parseLogicalOrExpression, ctx);
    if (!expr || !tryParse(token, ctx, /\?/)) return expr;
    const ifExpr = tryParse(parseConditionalExpression, ctx);
    if (!tryParse(token, ctx, /:/)) return null;
    const elseExpr = tryParse(parseConditionalExpression, ctx);
    return {
        type: 'conditional',
        test: expr,
        consequent: ifExpr,
        alternate: elseExpr
    };
}

function parseInclusiveOrExpression(ctx) {
    let left = tryParse(parseExclusiveOrExpression, ctx);
    if (!left) return null;
    while (tryParse(token, ctx, /\|/)) {
        const right = tryParse(parseExclusiveOrExpression, ctx);
        if (!right) return left;
        left = {type: 'binary', operator: '|', left, right};
    }
    return left;
}


// Logical OR parser
function parseLogicalOrExpression(ctx) {
    return parseBinaryExpression(ctx, parseLogicalAndExpression, '||');
}

// Logical AND parser
function parseLogicalAndExpression(ctx) {
    return parseBinaryExpression(ctx, parseInclusiveOrExpression, '&&');
}

// Generic binary expression parser
function parseBinaryExpression(ctx, operandParser, operatorToken) {
    let left = tryParse(operandParser, ctx);
    if (!left) return null;
    while (tryParse(token, ctx, new RegExp(operatorToken.replace(/[.*+?^${}()|[\]\\]/g, '\\$&')))) {
        const right = tryParse(operandParser, ctx);
        if (!right) return left;
        left = {type: 'binary', operator: operatorToken, left, right};
    }
    return left;
}

// Primary expression parser
function parsePrimaryExpression(ctx) {
    return (
        tryParse(token, ctx, TOKENS.IDENTIFIER) ||
        tryParse(parseConstant, ctx) ||
        tryParse(parseStringLiteral, ctx) ||
        tryParse(parseParenthesizedExpression, ctx) ||
        tryParse(parseMessageExpression, ctx) ||
        tryParse(parseEncodeExpression, ctx) ||
        tryParse(parseSelectorExpression, ctx)
    );
}

// Message expression parser
function parseMessageExpression(ctx) {
    if (!tryParse(token, ctx, /\[/)) return null;
    const receiver = tryParse(parseReceiver, ctx);
    if (!receiver) return null;
    const selector = tryParse(parseMessageSelector, ctx);
    if (!selector) return null;
    if (!tryParse(token, ctx, /\]/)) return null;
    return {type: 'messageExpression', receiver, selector};
}

// Receiver parser
function parseReceiver(ctx) {
    const expr = tryParse(parseExpression, ctx);
    if (expr) return {type: 'expression', expr};
    const type = tryParse(parseTypeSpecifier, ctx);
    if (type) return {type: 'type', specifier: type};
    return null;
}

// Message selector parser
function parseMessageSelector(ctx) {
    const keywords = [];
    while (true) {
        const keyword = tryParse(parseKeywordArgument, ctx);
        if (!keyword) break;
        keywords.push(keyword);
    }
    if (keywords.length === 0) return null;
    return keywords;
}

function parseKeywordArgument(ctx) {
    const selector = tryParse(parseSelector, ctx);
    if (!selector) return null;
    if (!tryParse(token, ctx, /:/)) return null;

    const argument = tryParse(parseKeywordArgumentType, ctx);
    if (!argument) return null;

    return { selector, argument };
}

// Implement missing parser stubs
function parseConstant(ctx) {
    return (
        tryParse(token, ctx, TOKENS.HEX_LITERAL) ||
        tryParse(token, ctx, TOKENS.OCTAL_LITERAL) ||
        tryParse(token, ctx, TOKENS.BINARY_LITERAL) ||
        tryParse(token, ctx, TOKENS.FLOATING_POINT_LITERAL) ||
        tryParse(token, ctx, TOKENS.DECIMAL_LITERAL) ||
        tryParse(token, ctx, TOKENS.CHARACTER_LITERAL) ||
        tryParse(token, ctx, /nil|NULL|YES|NO|true|false/)
    );
}
function parseStringLiteral(ctx) {
    return tryParse(token, ctx, TOKENS.STRING_LITERAL);
}
function parseParenthesizedExpression(ctx) {
    const start = ctx.index;
    if (!tryParse(token, ctx, /\(/)) return null;
    const expr = tryParse(parseExpression, ctx);
    if (!expr || !tryParse(token, ctx, /\)/)) {
        ctx.index = start;
        return null;
    }
    return {type: 'paren', expr};
}
function parseEncodeExpression(ctx) {
    const start = ctx.index;
    if (!tryParse(token, ctx, /@encode/)) return null;
    if (!tryParse(token, ctx, /\(/)) return null;
    // Capture typeName
    let typeName = '';
    while (ctx.index < ctx.length && ctx.input[ctx.index] !== ')') {
        typeName += ctx.input[ctx.index++];
    }
    if (!tryParse(token, ctx, /\)/)) { ctx.index = start; return null; }
    typeName = typeName.trim();
    return {type: 'encode', typeName};
}
function parseSelectorExpression(ctx) {
    const start = ctx.index;
    if (!tryParse(token, ctx, /@selector/)) return null;
    if (!tryParse(token, ctx, /\(/)) return null;
    // Capture selectorName
    let selectorName = '';
    const selStart = ctx.index;
    while (ctx.index < ctx.length && ctx.input[ctx.index] !== ')') {
        selectorName += ctx.input[ctx.index++];
    }
    if (!tryParse(token, ctx, /\)/)) { ctx.index = start; return null; }
    selectorName = selectorName.trim();
    return {type: 'selectorExpr', selectorName};
}
function parseTypeSpecifier(ctx) {
    // Try keywords, struct/union/enum, identifier
    return (
        tryParse(token, ctx, /void|char|short|int|long|float|double|id|BOOL|Class|IMP|SEL/) ||
        tryParse(parseStructOrUnionSpecifier, ctx) ||
        tryParse(parseEnumSpecifier, ctx) ||
        tryParse(token, ctx, TOKENS.IDENTIFIER)
    );
}
function parseTypeName(ctx) {
    // typeSpecifier ( '*' )*
    const type = tryParse(parseTypeSpecifier, ctx);
    if (!type) return null;
    // Handle pointer stars iteratively
    let stars = 0;
    while (tryParse(token, ctx, /\*/)) {
        stars++;
    }
    return {type: 'typeName', type, stars};
}
function parseDeclarator(ctx) {
    // (identifier | (LP declarator RP)) ( '[' constantExpression? ']' )*
    let decl = tryParse(token, ctx, TOKENS.IDENTIFIER);
    if (!decl && tryParse(token, ctx, /\(/)) {
        decl = tryParse(parseDeclarator, ctx);
        if (!tryParse(token, ctx, /\)/)) return null;
    }
    // Handle array declarators iteratively
    while (tryParse(token, ctx, /\[/)) {
        tryParse(parseConstantExpression, ctx);
        if (!tryParse(token, ctx, /\]/)) return null;
        decl = {type: 'arrayDeclarator', decl};
    }
    return decl ? {type: 'declarator', decl} : null;
}
function parseParameterDeclarationList(ctx) {
    // parameterDeclaration (',' parameterDeclaration)*
    const params = [];
    let param = tryParse(parseParameterDeclaration, ctx);
    if (!param) return null;
    params.push(param);
    while (tryParse(token, ctx, /,/)) {
        param = tryParse(parseParameterDeclaration, ctx);
        if (!param) break;
        params.push(param);
    }
    return params;
}
function parseParameterDeclaration(ctx) {
    // typeSpecifier declarator | 'void'
    if (tryParse(token, ctx, /void/)) return {type: 'void'};
    const type = tryParse(parseTypeSpecifier, ctx);
    if (!type) return null;
    const decl = tryParse(parseDeclarator, ctx);
    return {type, decl};
}
function parseStructOrUnionSpecifier(ctx) {
    // ('struct' | 'union') (identifier | identifier? '{' fieldDeclaration+ '}')
    if (!tryParse(token, ctx, /struct|union/)) return null;
    tryParse(token, ctx, TOKENS.IDENTIFIER);
    if (tryParse(token, ctx, /\{/)) {
        while (!tryParse(token, ctx, /\}/) && ctx.index < ctx.length) {
            tryParse(parseFieldDeclaration, ctx);
        }
    }
    return {type: 'structOrUnion'};
}
function parseFieldDeclaration(ctx) {
    // typeSpecifier fieldDeclaratorList macro? ';'
    tryParse(parseTypeSpecifier, ctx);
    tryParse(parseFieldDeclaratorList, ctx);
    // TODO: macro
    tryParse(token, ctx, /;/);
    return {type: 'fieldDecl'};
}
function parseFieldDeclaratorList(ctx) {
    // fieldDeclarator (',' fieldDeclarator)*
    tryParse(parseFieldDeclarator, ctx);
    while (tryParse(token, ctx, /,/)) {
        tryParse(parseFieldDeclarator, ctx);
    }
    return {type: 'fieldDeclList'};
}
function parseFieldDeclarator(ctx) {
    // declarator | declarator? ':' constant
    tryParse(parseDeclarator, ctx);
    if (tryParse(token, ctx, /:/)) tryParse(parseConstant, ctx);
    return {type: 'fieldDeclarator'};
}
function parseEnumSpecifier(ctx) {
    if (!tryParse(token, ctx, /enum/)) return null;
    if (!tryParse(token, ctx, TOKENS.IDENTIFIER)) return null;
    return {type: 'enum'};
}

function parseArrayInitializer(ctx) {
    // '{' (expression ','?)? '}'
    if (!tryParse(token, ctx, /\{/)) return null;
    tryParse(parseExpression, ctx);
    tryParse(token, ctx, /,/);
    if (!tryParse(token, ctx, /\}/)) return null;
    return {type: 'arrayInit'};
}
function parseStructInitializer(ctx) {
    // '{' ('.' structAssignmentExpression (',' '.' structAssignmentExpression)* ','?)? '}'
    if (!tryParse(token, ctx, /\{/)) return null;
    if (tryParse(token, ctx, /\./)) {
        tryParse(parseStructAssignmentExpression, ctx);
        while (tryParse(token, ctx, /,/) && tryParse(token, ctx, /\./)) {
            tryParse(parseStructAssignmentExpression, ctx);
        }
        tryParse(token, ctx, /,/);
    }
    if (!tryParse(token, ctx, /\}/)) return null;
    return {type: 'structInit'};
}
function parseStructAssignmentExpression(ctx) {
    // identifier '=' assignmentExpression
    tryParse(token, ctx, TOKENS.IDENTIFIER);
    tryParse(token, ctx, /=/);
    tryParse(parseAssignmentExpression, ctx);
    return {type: 'structAssign'};
}
function parseInitializerList(ctx) {
    // initializer (',' initializer)* ','?
    tryParse(parseInitializer, ctx);
    while (tryParse(token, ctx, /,/)) {
        tryParse(parseInitializer, ctx);
    }
    tryParse(token, ctx, /,/);
    return {type: 'initList'};
}
function parseInitializer(ctx) {
    // assignmentExpression | arrayInitializer | structInitializer
    return (
        tryParse(parseAssignmentExpression, ctx) ||
        tryParse(parseArrayInitializer, ctx) ||
        tryParse(parseStructInitializer, ctx)
    );
}
function parseSelector(ctx) {
    return (
        tryParse(token, ctx, TOKENS.IDENTIFIER) ||
        tryParse(token, ctx, /return/)
    );
}

function parseKeywordArgumentType(ctx) {
    // expression ('{' initializerList '}')?
    const expr = tryParse(parseExpression, ctx);
    if (!expr) return null;
    const start = ctx.index;
    if (tryParse(token, ctx, /{/)) {
        const initList = tryParse(parseInitializerList, ctx);
        if (!tryParse(token, ctx, /}/)) {
            ctx.index = start;
            return expr;
        }
        return {expr, initList};
    }
    return expr;
}
function parseArgumentExpressionList(ctx) {
    // argumentExpression (',' argumentExpression)*
    const args = [];
    let arg = tryParse(parseArgumentExpression, ctx);
    if (!arg) return args;
    args.push(arg);
    while (tryParse(token, ctx, /,/)) {
        arg = tryParse(parseArgumentExpression, ctx);
        if (!arg) break;
        args.push(arg);
    }
    return args;
}
function parseArgumentExpression(ctx) {
    // expression | typeSpecifier
    return (
        tryParse(parseExpression, ctx) ||
        tryParse(parseTypeSpecifier, ctx)
    );
}
function parseConstantExpression(ctx) {
    return tryParse(parseConditionalExpression, ctx);
}

function parseExclusiveOrExpression(ctx) {
    let left = tryParse(parseAndExpression, ctx);
    if (!left) return null;
    while (tryParse(token, ctx, /\^/)) {
        const right = tryParse(parseAndExpression, ctx);
        if (!right) return left;
        left = {type: 'binary', operator: '^', left, right};
    }
    return left;
}

function parseAndExpression(ctx) {
    let left = tryParse(parseEqualityExpression, ctx);
    if (!left) return null;
    while (tryParse(token, ctx, /&/)) {
        const right = tryParse(parseEqualityExpression, ctx);
        if (!right) return left;
        left = {type: 'binary', operator: '&', left, right};
    }
    return left;
}

function parseEqualityExpression(ctx) {
    let left = tryParse(parseRelationalExpression, ctx);
    if (!left) return null;
    while (true) {
        if (tryParse(token, ctx, /==/)) {
            const right = tryParse(parseRelationalExpression, ctx);
            if (!right) return left;
            left = {type: 'binary', operator: '==', left, right};
        } else if (tryParse(token, ctx, /!=/)) {
            const right = tryParse(parseRelationalExpression, ctx);
            if (!right) return left;
            left = {type: 'binary', operator: '!=', left, right};
        } else {
            break;
        }
    }
    return left;
}

function parseRelationalExpression(ctx) {
    let left = tryParse(parseShiftExpression, ctx);
    if (!left) return null;
    while (true) {
        if (tryParse(token, ctx, /<=/)) {
            const right = tryParse(parseShiftExpression, ctx);
            if (!right) return left;
            left = {type: 'binary', operator: '<=', left, right};
        } else if (tryParse(token, ctx, />=/)) {
            const right = tryParse(parseShiftExpression, ctx);
            if (!right) return left;
            left = {type: 'binary', operator: '>=', left, right};
        } else if (tryParse(token, ctx, /</)) {
            const right = tryParse(parseShiftExpression, ctx);
            if (!right) return left;
            left = {type: 'binary', operator: '<', left, right};
        } else if (tryParse(token, ctx, />/)) {
            const right = tryParse(parseShiftExpression, ctx);
            if (!right) return left;
            left = {type: 'binary', operator: '>', left, right};
        } else {
            break;
        }
    }
    return left;
}

function parseShiftExpression(ctx) {
    let left = tryParse(parseAdditiveExpression, ctx);
    if (!left) return null;
    while (true) {
        if (tryParse(token, ctx, /<</)) {
            const right = tryParse(parseAdditiveExpression, ctx);
            if (!right) return left;
            left = {type: 'binary', operator: '<<', left, right};
        } else if (tryParse(token, ctx, />>/)) {
            const right = tryParse(parseAdditiveExpression, ctx);
            if (!right) return left;
            left = {type: 'binary', operator: '>>', left, right};
        } else {
            break;
        }
    }
    return left;
}

function parseAdditiveExpression(ctx) {
    let left = tryParse(parseMultiplicativeExpression, ctx);
    if (!left) return null;
    while (true) {
        if (tryParse(token, ctx, /\+/)) {
            const right = tryParse(parseMultiplicativeExpression, ctx);
            if (!right) return left;
            left = {type: 'binary', operator: '+', left, right};
        } else if (tryParse(token, ctx, /-/)) {
            const right = tryParse(parseMultiplicativeExpression, ctx);
            if (!right) return left;
            left = {type: 'binary', operator: '-', left, right};
        } else {
            break;
        }
    }
    return left;
}

function parseMultiplicativeExpression(ctx) {
    let left = tryParse(parseCastExpression, ctx);
    if (!left) return null;
    while (true) {
        if (tryParse(token, ctx, /\*/)) {
            const right = tryParse(parseCastExpression, ctx);
            if (!right) return left;
            left = {type: 'binary', operator: '*', left, right};
        } else if (tryParse(token, ctx, /\//)) {
            const right = tryParse(parseCastExpression, ctx);
            if (!right) return left;
            left = {type: 'binary', operator: '/', left, right};
        } else if (tryParse(token, ctx, /%/)) {
            const right = tryParse(parseCastExpression, ctx);
            if (!right) return left;
            left = {type: 'binary', operator: '%', left, right};
        } else {
            break;
        }
    }
    return left;
}

function parseCastExpression(ctx) {
    // (LP typeName RP) castExpression | unaryExpression
    const start = ctx.index;
    if (tryParse(token, ctx, /\(/)) {
        // TODO: typeName parsing, for now just skip to ')'
        // In a real parser, call tryParse(parseTypeName, ctx)
        while (!tryParse(token, ctx, /\)/) && ctx.index < ctx.length) {
            ctx.index++;
        }
        const expr = tryParse(parseCastExpression, ctx);
        if (expr) return {type: 'cast', expr};
        ctx.index = start;
    }
    ctx.index = start;
    return tryParse(parseUnaryExpression, ctx);
}

function parseUnaryExpression(ctx) {
    // ('++' | '--')* (postfixExpression | unaryOperator castExpression | ...)
    let op;
    let count = 0;
    while ((op = tryParse(token, ctx, /\+\+|--/))) {
        count++;
    }
    // Try unary operator
    const unaryOps = [/&/, /\*/, /\+/, /-/, /~/, /!/];
    for (let i = 0; i < unaryOps.length; ++i) {
        if (tryParse(token, ctx, unaryOps[i])) {
            const expr = tryParse(parseCastExpression, ctx);
            if (expr) return {type: 'unary', operator: unaryOps[i].toString(), expr};
            return null;
        }
    }
    // Fallback to postfix
    let expr = tryParse(parsePostfixExpression, ctx);
    if (count > 0 && expr) {
        for (let i = 0; i < count; ++i) {
            expr = {type: 'unary', operator: '++/--', expr};
        }
    }
    return expr;
}

function parsePostfixExpression(ctx) {
    // (primaryExpression | ... ) ( '[' expression ']' | '(' argumentExpressionList? ')' | '.' identifier | '->' identifier | '++' | '--' )*
    let expr = tryParse(parsePrimaryExpression, ctx);
    if (!expr) return null;
    while (true) {
        if (tryParse(token, ctx, /\[/)) {
            const index = tryParse(parseExpression, ctx);
            if (!tryParse(token, ctx, /\]/)) return null;
            expr = {type: 'subscript', e: expr, index};
        } else if (tryParse(token, ctx, /\(/)) {
            const args = tryParse(parseArgumentExpressionList, ctx);
            if (!tryParse(token, ctx, /\)/)) return null;
            expr = {type: 'call', e: expr, args: args};
        } else if (tryParse(token, ctx, /\./)) {
            const id = tryParse(token, ctx, TOKENS.IDENTIFIER);
            if (!id) return null;
            expr = {type: 'dot', e: expr, id: id};
        } else if (tryParse(token, ctx, /->/)) {
            const id = tryParse(token, ctx, TOKENS.IDENTIFIER);
            if (!id) return null;
            expr = {type: 'arrow', e: expr, id: id};
        } else if (tryParse(token, ctx, /\+\+/)) {
            expr = {type: 'postfix', operator: '++', expr};
        } else if (tryParse(token, ctx, /--/)) {
            expr = {type: 'postfix', operator: '--', expr};
        } else {
            break;
        }
    }
    return expr;
}

function parseImplementation(ctx) {
    const start = ctx.index;
    if (!tryParse(token, ctx, /@implementation/)) return null;
    const name = tryParse(token, ctx, TOKENS.IDENTIFIER);
    if (!name) { ctx.index = start; return null; }
    // Optionally superclass
    tryParse(token, ctx, /:/);
    tryParse(token, ctx, TOKENS.IDENTIFIER);
    // Methods and raw content until @end
    let members = [];
    let rawText = '';
    while (ctx.index < ctx.length && !tryParse(token, ctx, /@end/)) {
        const method = tryParse(parseMethod, ctx);
        if (method) {
            if (rawText.length) { members.push({type: 'raw', text: rawText}); rawText = ''; }
            members.push(method);
        } else {
            rawText += ctx.input[ctx.index++];
        }
    }
    if (rawText.length) members.push({type: 'raw', text: rawText});
    return {type: 'implementation', name, members};
}

function parseInterface(ctx) {
    const start = ctx.index;
    if (!tryParse(token, ctx, /@interface/)) return null;
    const name = tryParse(token, ctx, TOKENS.IDENTIFIER);
    if (!name) { ctx.index = start; return null; }
    // Optionally superclass
    tryParse(token, ctx, /:/);
    tryParse(token, ctx, TOKENS.IDENTIFIER);
    // Optionally ivar block
    let ivars = [];
    if (tryParse(token, ctx, /\{/)) {
        let ivarText = '';
        while (!tryParse(token, ctx, /\}/) && ctx.index < ctx.length) {
            ivarText += ctx.input[ctx.index++];
        }
        ivars.push({type: 'raw', text: ivarText});
    }
    // Methods and raw content until @end
    let members = [];
    let rawText = '';
    while (ctx.index < ctx.length && !tryParse(token, ctx, /@end/)) {
        const method = tryParse(parseMethod, ctx);
        if (method) {
            if (rawText.length) { members.push({type: 'raw', text: rawText}); rawText = ''; }
            members.push(method);
        } else {
            rawText += ctx.input[ctx.index++];
        }
    }
    if (rawText.length) members.push({type: 'raw', text: rawText});
    return {type: 'interface', name, ivars, members};
}

function parseMethod(ctx) {
    // Parse + or -
    const start = ctx.index;
    const kind = tryParse(token, ctx, /[+-]/);
    if (!kind) return null;
    // Parse return type in parentheses
    if (!tryParse(token, ctx, /\(/)) { ctx.index = start; return null; }
    let returnType = '';
    while (ctx.index < ctx.length && ctx.input[ctx.index] !== ')') {
        returnType += ctx.input[ctx.index++];
    }
    if (!tryParse(token, ctx, /\)/)) { ctx.index = start; return null; }
    returnType = returnType.trim();
    // Parse selector and arguments
    let selector = [];
    let args = [];
    // At least one selector part
    let selPart = '';
    // Parse selector part (identifier)
    const selId = tryParse(token, ctx, TOKENS.IDENTIFIER);
    if (!selId) { ctx.index = start; return null; }
    selPart = selId;
    // Check for argument (':' and type/name)
    if (tryParse(token, ctx, /:/)) {
        selector.push(selPart + ':');
        // Parse argument type in parentheses
        if (!tryParse(token, ctx, /\(/)) { ctx.index = start; return null; }
        let argType = '';
        while (ctx.index < ctx.length && ctx.input[ctx.index] !== ')') {
            argType += ctx.input[ctx.index++];
        }
        if (!tryParse(token, ctx, /\)/)) { ctx.index = start; return null; }
        argType = argType.trim();
        // Parse argument name
        const argName = tryParse(token, ctx, TOKENS.IDENTIFIER);
        if (!argName) { ctx.index = start; return null; }
        args.push({type: argType, name: argName});
        // Parse additional selector parts/arguments
        while (true) {
            // Next selector part
            const nextSel = tryParse(token, ctx, TOKENS.IDENTIFIER);
            if (!nextSel) break;
            if (!tryParse(token, ctx, /:/)) { ctx.index -= nextSel.length; break; }
            selector.push(nextSel + ':');
            if (!tryParse(token, ctx, /\(/)) { ctx.index = start; return null; }
            let nextArgType = '';
            while (ctx.index < ctx.length && ctx.input[ctx.index] !== ')') {
                nextArgType += ctx.input[ctx.index++];
            }
            if (!tryParse(token, ctx, /\)/)) { ctx.index = start; return null; }
            nextArgType = nextArgType.trim();
            const nextArgName = tryParse(token, ctx, TOKENS.IDENTIFIER);
            if (!nextArgName) { ctx.index = start; return null; }
            args.push({type: nextArgType, name: nextArgName});
        }
    } else {
        selector.push(selPart);
    }
    // Skip to next semicolon or method body
    skipWhitespace(ctx);
    if (tryParse(token, ctx, /;/)) {
        return {type: 'method', kind, returnType, selector, args};
    } else if (tryParse(token, ctx, /\{/)) {
        // Parse method body as raw text until matching }
        let depth = 1;
        let body = '';
        while (ctx.index < ctx.length && depth > 0) {
            if (ctx.input[ctx.index] === '{') depth++;
            else if (ctx.input[ctx.index] === '}') depth--;
            if (depth > 0) body += ctx.input[ctx.index];
            ctx.index++;
        }
        return {type: 'method', kind, returnType, selector, args, body};
    } else {
        // Unexpected end
        ctx.index = start;
        return null;
    }
}

function parseTopLevel(ctx) {
    let nodes = [];
    let rawText = "";
    while (ctx.input[ctx.index])
    {
        let node = tryParse(parseInterface, ctx)
            || tryParse(parseImplementation, ctx)
            || tryParse(parseExpression, ctx)
        if (node) {
            // flush
            if (rawText.length)
                nodes.push({type: 'raw', text: rawText})
            rawText = "";
            nodes.push(node);
        } else {
            rawText += ctx.input[ctx.index++];
        }
    }
    if (rawText.length)
        nodes.push({type: 'raw', text: rawText})
    return nodes;
}

// Example usage
const objcSource = `
[foo performSelector:12 name:bar]
`;
const ctx = new ParseContext(objcSource);
const ast = parseMessageExpression(ctx);
console.log(JSON.stringify(ast, null, 2));