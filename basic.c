#include "./basic.h"

void sb_resize(StringBuilder* sb, size_t new_capacity)
{
    	// Adding plus 1 for null character
    	sb->capacity = new_capacity+1;
	sb->length = (sb->capacity < sb->length)? sb->capacity: sb->length;

    	if (sb->capacity > 0)
    	{
		char* new_data = (char*)malloc(sb->capacity);
		assert(new_data != NULL && "Unable to allocate memory");
		memset(new_data, 0, sb->capacity);
		memmove(new_data, sb->data, sb->length);
		sb->data = new_data;
    	}
}

void sb_push_str(StringBuilder* sb, char* str)
{
    	size_t new_str_len = strlen(str);
	if (sb->capacity < (sb->length + new_str_len + 1)) {
    		sb_resize(sb, sb->capacity + new_str_len);
	}

	memmove(sb->data+sb->length, str, new_str_len);
	sb->length += new_str_len;
}

void sb_push_char(StringBuilder* sb, char ch)
{
	if (sb->capacity < (sb->length + 2)) {
    		sb_resize(sb, sb->capacity);
	}

	sb->data[sb->length] = ch;
	sb->length += 1;
}

void sb_push_int(StringBuilder* sb, int i)
{
    	if (i == 0) 
    	{
        	sb_push_char(sb, '0');
        	return;
    	}
    	
	int k = (i<0)? log10(-i)+2 : log10(i)+1;
	sb_resize(sb, sb->capacity+k);

	int j = k; 
	while (i != 0) 
    	{
		sb->data[sb->length + j - 1] = i%10 + '0';
		i = i/10;
		j -= 1;
	}

	if (i<0) sb->data[sb->length] = '-';
	sb->length += k;
}

void sb_push_float(StringBuilder* sb, float f)
{	
    	// Handling int part
    	int i = floorf(f);
	sb_push_int(sb, i);

	// Handling fractional part
	sb_push_char(sb, '.');

	int d = (f-i)*1000000;
	sb_push_int(sb, d);
}

StringBuilder sb_clone(StringBuilder* sb)
{	
    	StringBuilder clone = {0};
 	sb_push_str(&clone, sb->data);
    	return clone; 
}

StringView sv_new(char* str, size_t len)
{
    return (StringView){.length=len, .data=str};
}

StringView sv_from_cstr(char* str)
{
	StringView sv = {0}; 
	sv.data = str;
	sv.length = strlen(str);
    	return sv;
}

StringView sv_from_sb(StringBuilder *sb)
{
	StringView sv = {0};
	sv.data = sb->data;
	sv.length = sb->length;
    	return sv;
}

bool sv_equal(StringView s1, StringView s2)
{
    if (s1.length != s2.length) return false;

    for (size_t i=0; i<s1.length; i++)
    {
        if (s1.data[i] != s2.data[i])
            return false;
    }

    return true;
}

void sv_trim_left(StringView *sv)
{
    	while (isspace(*sv->data))
        {
        	sv->data++;
        	sv->length--;
        }
}

void sv_trim_right(StringView* sv)
{
    	while(isspace(sv->data[sv->length-1]))
        	sv->length--;
}

void sv_trim(StringView *sv)
{
    sv_trim_left(sv);
    sv_trim_right(sv);
}

StringView sv_chop_delim(StringView* sv, char delim)
{
    	size_t i = 0;
    	while (sv->data[i] != delim && i < sv->length)
        	i++;

    	StringView result = {0};
	result.data = sv->data;
	result.length = i;
	
    	sv->data = sv->data+i+1;
    	sv->length = sv->length-i;

	return result;
}

StringView sv_chop_str(StringView* sv, char* str)
{
    size_t n = strlen(str);
    StringView pat = sv_new(str, n);
    StringView result = sv_new(sv->data, 0);

    size_t i = 0;
    for (i=0; i<sv->length-n; i++)
    {
	StringView target = sv_new(&(sv->data[i]), n);
	if (sv_equal(pat, target)) break;
    }

    result.length = i;

    sv->data = sv->data+i+n;
    sv->length = sv->length-i-n;
    
    return result;
}
