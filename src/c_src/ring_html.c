#include "ring.h"

#include <lexbor/html/html.h>

#include <lexbor/css/css.h>
#include <lexbor/selectors/selectors.h>

#define RING_HTML_DOC "HTML_DOC"
#define RING_HTML_NODE "HTML_NODE"

void free_html_doc(void *pState, void *pDoc)
{
	if (pDoc != NULL)
	{
		lxb_html_document_destroy((lxb_html_document_t *)pDoc);
	}
}

RING_FUNC(ring_html_parse)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISSTRING(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	const lxb_char_t *html = (const lxb_char_t *)RING_API_GETSTRING(1);
	size_t html_len = (size_t)RING_API_GETSTRINGSIZE(1);
	lxb_html_document_t *document = lxb_html_document_create();
	if (document == NULL)
	{
		RING_API_ERROR("Failed to create Lexbor document.");
		return;
	}
	lxb_status_t status = lxb_html_document_parse(document, html, html_len);
	if (status != LXB_STATUS_OK)
	{
		lxb_html_document_destroy(document);
		RING_API_ERROR("Failed to parse HTML.");
		return;
	}

	RING_API_RETMANAGEDCPOINTER(document, RING_HTML_DOC, free_html_doc);
}

RING_FUNC(ring_html_get_body)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_html_document_t *document = (lxb_html_document_t *)RING_API_GETCPOINTER(1, RING_HTML_DOC);
	if (document != NULL)
	{
		lxb_html_body_element_t *body = lxb_html_document_body_element(document);
		if (body != NULL)
		{

			RING_API_RETMANAGEDCPOINTER(body, RING_HTML_NODE, NULL);
		}
	}
}

static lxb_status_t find_callback(lxb_dom_node_t *node, lxb_css_selector_specificity_t spec, void *ctx)
{
	lxb_dom_collection_t *collection = (lxb_dom_collection_t *)ctx;
	return lxb_dom_collection_append(collection, node);
}

RING_FUNC(ring_html_find)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISCPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}

	lxb_dom_node_t *root_node = NULL;
	lxb_html_document_t *doc_ptr = NULL;

	List *pList = RING_API_GETLIST(1);
	if (ring_vm_api_iscpointerlist(RING_API_STATE, pList))
	{
		const char *cType = ring_list_getstring(pList, 2);
		if (strcmp(cType, RING_HTML_DOC) == 0)
		{
			doc_ptr = (lxb_html_document_t *)ring_list_getpointer(pList, 1);
			root_node = lxb_dom_interface_node(doc_ptr);
		}
		else if (strcmp(cType, RING_HTML_NODE) == 0)
		{
			root_node = (lxb_dom_node_t *)ring_list_getpointer(pList, 1);
			doc_ptr = (lxb_html_document_t *)root_node->owner_document;
		}
	}

	if (root_node == NULL || doc_ptr == NULL)
	{
		RING_API_ERROR("Invalid document or node pointer provided to find().");
		return;
	}

	const lxb_char_t *selector_str = (const lxb_char_t *)RING_API_GETSTRING(2);
	size_t selector_len = (size_t)RING_API_GETSTRINGSIZE(2);

	lxb_status_t status;
	lxb_css_parser_t *parser = lxb_css_parser_create();
	status = lxb_css_parser_init(parser, NULL);

	lxb_css_selector_list_t *list = lxb_css_selectors_parse(parser, selector_str, selector_len);
	if (list == NULL)
	{
		RING_API_ERROR("Failed to parse CSS selector.");
		lxb_css_parser_destroy(parser, true);
		return;
	}

	lxb_selectors_t *selectors = lxb_selectors_create();
	status = lxb_selectors_init(selectors);
	if (status != LXB_STATUS_OK)
	{
		RING_API_ERROR("Failed to init selectors object.");
		lxb_css_selector_list_destroy(list);
		lxb_css_parser_destroy(parser, true);
		lxb_selectors_destroy(selectors, true);
		return;
	}

	lxb_dom_collection_t *collection = lxb_dom_collection_make(&doc_ptr->dom_document, 16);

	status = lxb_selectors_find(selectors, root_node, list, find_callback, collection);

	List *pResultList = RING_API_NEWLIST;

	if (collection != NULL)
	{
		for (size_t i = 0; i < lxb_dom_collection_length(collection); i++)
		{
			lxb_dom_element_t *element = lxb_dom_collection_element(collection, i);
			List *pNodeList = ring_list_newlist_gc(RING_API_STATE, pResultList);

			ring_list_addcpointer_gc(RING_API_STATE, pNodeList, element, RING_HTML_NODE);
		}
	}

	lxb_dom_collection_destroy(collection, true);
	lxb_css_selector_list_destroy(list);
	lxb_selectors_destroy(selectors, true);
	lxb_css_parser_destroy(parser, true);

	RING_API_RETLIST(pResultList);
}

RING_FUNC(ring_html_find_attr)
{
	if (RING_API_PARACOUNT != 3 || !RING_API_ISCPOINTER(1) || !RING_API_ISSTRING(2) || !RING_API_ISSTRING(3))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}

	lxb_dom_node_t *root_node = NULL;
	lxb_html_document_t *doc_ptr = NULL;

	List *pList = RING_API_GETLIST(1);
	if (ring_vm_api_iscpointerlist(RING_API_STATE, pList))
	{
		const char *cType = ring_list_getstring(pList, 2);
		if (strcmp(cType, RING_HTML_DOC) == 0)
		{
			doc_ptr = (lxb_html_document_t *)ring_list_getpointer(pList, 1);
			root_node = lxb_dom_interface_node(doc_ptr);
		}
		else if (strcmp(cType, RING_HTML_NODE) == 0)
		{
			root_node = (lxb_dom_node_t *)ring_list_getpointer(pList, 1);
			doc_ptr = (lxb_html_document_t *)root_node->owner_document;
		}
	}

	if (root_node == NULL || doc_ptr == NULL)
	{
		RING_API_ERROR("Invalid document or node pointer provided to find_attr().");
		return;
	}

	const lxb_char_t *selector_str = (const lxb_char_t *)RING_API_GETSTRING(2);
	size_t selector_len = (size_t)RING_API_GETSTRINGSIZE(2);
	const lxb_char_t *attr_name = (const lxb_char_t *)RING_API_GETSTRING(3);
	size_t attr_name_len = (size_t)RING_API_GETSTRINGSIZE(3);

	lxb_status_t status;
	lxb_css_parser_t *parser = lxb_css_parser_create();
	status = lxb_css_parser_init(parser, NULL);

	lxb_css_selector_list_t *list = lxb_css_selectors_parse(parser, selector_str, selector_len);
	if (list == NULL)
	{
		RING_API_ERROR("Failed to parse CSS selector.");
		lxb_css_parser_destroy(parser, true);
		return;
	}

	lxb_selectors_t *selectors = lxb_selectors_create();
	status = lxb_selectors_init(selectors);
	if (status != LXB_STATUS_OK)
	{
		RING_API_ERROR("Failed to init selectors object.");
		lxb_css_selector_list_destroy(list);
		lxb_css_parser_destroy(parser, true);
		lxb_selectors_destroy(selectors, true);
		return;
	}

	lxb_dom_collection_t *collection = lxb_dom_collection_make(&doc_ptr->dom_document, 16);

	status = lxb_selectors_find(selectors, root_node, list, find_callback, collection);

	List *pResultList = RING_API_NEWLIST;

	if (collection != NULL)
	{
		for (size_t i = 0; i < lxb_dom_collection_length(collection); i++)
		{
			lxb_dom_element_t *element = lxb_dom_collection_element(collection, i);
			size_t value_len;
			const lxb_char_t *value = lxb_dom_element_get_attribute(element, attr_name, attr_name_len, &value_len);
			if (value != NULL)
			{
				ring_list_addstring2_gc(RING_API_STATE, pResultList, (const char *)value, (int)value_len);
			}
		}
	}

	lxb_dom_collection_destroy(collection, true);
	lxb_css_selector_list_destroy(list);
	lxb_selectors_destroy(selectors, true);
	lxb_css_parser_destroy(parser, true);

	RING_API_RETLIST(pResultList);
}

RING_FUNC(ring_html_node_text)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_node_t *node = (lxb_dom_node_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	if (node != NULL)
	{
		size_t len;
		const lxb_char_t *text = lxb_dom_node_text_content(node, &len);
		if (text != NULL)
		{
			RING_API_RETSTRING2((const char *)text, (int)len);
		}
	}
}

RING_FUNC(ring_html_node_html)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_node_t *node = (lxb_dom_node_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	if (node != NULL && node->type == LXB_DOM_NODE_TYPE_ELEMENT)
	{
		lexbor_str_t str = {0};
		lxb_status_t status = lxb_html_serialize_tree_str(node, &str);

		if (status == LXB_STATUS_OK)
		{
			RING_API_RETSTRING2((const char *)lexbor_str_data(&str), (int)lexbor_str_length(&str));
			lexbor_str_destroy(&str, node->owner_document->text, false);
		}
	}
}

RING_FUNC(ring_html_node_tag)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_element_t *element = (lxb_dom_element_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	if (element != NULL)
	{
		size_t len;
		const lxb_char_t *tag = lxb_dom_element_tag_name(element, &len);
		if (tag != NULL)
		{
			RING_API_RETSTRING2((const char *)tag, (int)len);
		}
	}
}

RING_FUNC(ring_html_node_attr)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISCPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_element_t *element = (lxb_dom_element_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	const lxb_char_t *attr_name = (const lxb_char_t *)RING_API_GETSTRING(2);
	size_t attr_name_len = (size_t)RING_API_GETSTRINGSIZE(2);

	if (element != NULL)
	{
		size_t value_len;
		const lxb_char_t *value = lxb_dom_element_get_attribute(element, attr_name, attr_name_len, &value_len);
		if (value != NULL)
		{
			RING_API_RETSTRING2((const char *)value, (int)value_len);
		}
	}
}

RING_FUNC(ring_html_node_parent)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_node_t *node = (lxb_dom_node_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	if (node != NULL && node->parent != NULL)
	{

		if (node->parent->type == LXB_DOM_NODE_TYPE_ELEMENT)
		{

			RING_API_RETMANAGEDCPOINTER(node->parent, RING_HTML_NODE, NULL);
		}
	}
}

RING_FUNC(ring_html_node_next_sibling)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_node_t *node = (lxb_dom_node_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	if (node != NULL)
	{
		lxb_dom_node_t *sibling = node->next;

		while (sibling != NULL && sibling->type != LXB_DOM_NODE_TYPE_ELEMENT)
		{
			sibling = sibling->next;
		}
		if (sibling != NULL)
		{

			RING_API_RETMANAGEDCPOINTER(sibling, RING_HTML_NODE, NULL);
		}
	}
}

RING_FUNC(ring_html_node_prev_sibling)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_node_t *node = (lxb_dom_node_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	if (node != NULL)
	{
		lxb_dom_node_t *sibling = node->prev;

		while (sibling != NULL && sibling->type != LXB_DOM_NODE_TYPE_ELEMENT)
		{
			sibling = sibling->prev;
		}
		if (sibling != NULL)
		{

			RING_API_RETMANAGEDCPOINTER(sibling, RING_HTML_NODE, NULL);
		}
	}
}

RING_FUNC(ring_html_node_children)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_node_t *node = (lxb_dom_node_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	List *pResultList = RING_API_NEWLIST;

	if (node != NULL)
	{
		lxb_dom_node_t *child = node->first_child;
		while (child != NULL)
		{

			if (child->type == LXB_DOM_NODE_TYPE_ELEMENT)
			{
				List *pNodeList = ring_list_newlist_gc(RING_API_STATE, pResultList);

				ring_list_addcpointer_gc(RING_API_STATE, pNodeList, child, RING_HTML_NODE);
			}
			child = child->next;
		}
	}
	RING_API_RETLIST(pResultList);
}

RING_FUNC(ring_html_node_has_attr)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISCPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_element_t *element = (lxb_dom_element_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	const lxb_char_t *attr_name = (const lxb_char_t *)RING_API_GETSTRING(2);
	size_t attr_name_len = (size_t)RING_API_GETSTRINGSIZE(2);

	if (element != NULL)
	{
		bool result = lxb_dom_element_has_attribute(element, attr_name, attr_name_len);
		RING_API_RETNUMBER(result);
	}
	else
	{
		RING_API_RETNUMBER(0);
	}
}

RING_FUNC(ring_html_create_element)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISCPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_html_document_t *document = (lxb_html_document_t *)RING_API_GETCPOINTER(1, RING_HTML_DOC);
	const lxb_char_t *tag_name = (const lxb_char_t *)RING_API_GETSTRING(2);
	size_t tag_name_len = (size_t)RING_API_GETSTRINGSIZE(2);

	if (document == NULL)
	{
		RING_API_ERROR("Invalid document pointer.");
		return;
	}

	lxb_dom_element_t *element = lxb_dom_document_create_element(&document->dom_document, tag_name, tag_name_len, NULL);
	if (element == NULL)
	{
		RING_API_ERROR("Failed to create element.");
		return;
	}

	RING_API_RETMANAGEDCPOINTER(element, RING_HTML_NODE, NULL);
}

RING_FUNC(ring_html_create_text_node)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISCPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_html_document_t *document = (lxb_html_document_t *)RING_API_GETCPOINTER(1, RING_HTML_DOC);
	const lxb_char_t *text = (const lxb_char_t *)RING_API_GETSTRING(2);
	size_t text_len = (size_t)RING_API_GETSTRINGSIZE(2);

	if (document == NULL)
	{
		RING_API_ERROR("Invalid document pointer.");
		return;
	}

	lxb_dom_text_t *text_node = lxb_dom_document_create_text_node(&document->dom_document, text, text_len);
	if (text_node == NULL)
	{
		RING_API_ERROR("Failed to create text node.");
		return;
	}

	RING_API_RETMANAGEDCPOINTER(text_node, RING_HTML_NODE, NULL);
}

RING_FUNC(ring_html_node_append_child)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISCPOINTER(1) || !RING_API_ISCPOINTER(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_node_t *parent = (lxb_dom_node_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	lxb_dom_node_t *child = (lxb_dom_node_t *)RING_API_GETCPOINTER(2, RING_HTML_NODE);

	if (parent == NULL || child == NULL)
	{
		RING_API_ERROR("Invalid node pointer.");
		return;
	}

	lxb_dom_node_insert_child(parent, child);
	RING_API_RETNUMBER(1);
}

RING_FUNC(ring_html_node_insert_before)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISCPOINTER(1) || !RING_API_ISCPOINTER(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_node_t *target = (lxb_dom_node_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	lxb_dom_node_t *new_node = (lxb_dom_node_t *)RING_API_GETCPOINTER(2, RING_HTML_NODE);

	if (target == NULL || new_node == NULL)
	{
		RING_API_ERROR("Invalid node pointer.");
		return;
	}

	lxb_dom_node_insert_before(target, new_node);
	RING_API_RETNUMBER(1);
}

RING_FUNC(ring_html_node_insert_after)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISCPOINTER(1) || !RING_API_ISCPOINTER(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_node_t *target = (lxb_dom_node_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	lxb_dom_node_t *new_node = (lxb_dom_node_t *)RING_API_GETCPOINTER(2, RING_HTML_NODE);

	if (target == NULL || new_node == NULL)
	{
		RING_API_ERROR("Invalid node pointer.");
		return;
	}

	lxb_dom_node_insert_after(target, new_node);
	RING_API_RETNUMBER(1);
}

RING_FUNC(ring_html_node_remove)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_node_t *node = (lxb_dom_node_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);

	if (node == NULL)
	{
		RING_API_ERROR("Invalid node pointer.");
		return;
	}

	lxb_dom_node_remove(node);
	RING_API_RETNUMBER(1);
}

RING_FUNC(ring_html_node_set_attr)
{
	if (RING_API_PARACOUNT != 3 || !RING_API_ISCPOINTER(1) || !RING_API_ISSTRING(2) || !RING_API_ISSTRING(3))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_element_t *element = (lxb_dom_element_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	const lxb_char_t *attr_name = (const lxb_char_t *)RING_API_GETSTRING(2);
	size_t attr_name_len = (size_t)RING_API_GETSTRINGSIZE(2);
	const lxb_char_t *attr_value = (const lxb_char_t *)RING_API_GETSTRING(3);
	size_t attr_value_len = (size_t)RING_API_GETSTRINGSIZE(3);

	if (element == NULL)
	{
		RING_API_ERROR("Invalid element pointer.");
		return;
	}

	lxb_dom_attr_t *attr = lxb_dom_element_set_attribute(element, attr_name, attr_name_len, attr_value, attr_value_len);
	if (attr == NULL)
	{
		RING_API_ERROR("Failed to set attribute.");
		return;
	}

	RING_API_RETNUMBER(1);
}

RING_FUNC(ring_html_node_remove_attr)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISCPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_element_t *element = (lxb_dom_element_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	const lxb_char_t *attr_name = (const lxb_char_t *)RING_API_GETSTRING(2);
	size_t attr_name_len = (size_t)RING_API_GETSTRINGSIZE(2);

	if (element == NULL)
	{
		RING_API_ERROR("Invalid element pointer.");
		return;
	}

	lxb_dom_element_remove_attribute(element, attr_name, attr_name_len);
	RING_API_RETNUMBER(1);
}

RING_FUNC(ring_html_node_get_all_attrs)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_element_t *element = (lxb_dom_element_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);

	List *pResultList = RING_API_NEWLIST;

	if (element != NULL)
	{
		lxb_dom_attr_t *attr = lxb_dom_element_first_attribute(element);
		while (attr != NULL)
		{
			size_t name_len, value_len;
			const lxb_char_t *name = lxb_dom_attr_local_name(attr, &name_len);
			const lxb_char_t *value = lxb_dom_attr_value(attr, &value_len);

			List *pAttrPair = ring_list_newlist_gc(RING_API_STATE, pResultList);
			ring_list_addstring2_gc(RING_API_STATE, pAttrPair, (const char *)name, (int)name_len);
			if (value != NULL)
			{
				ring_list_addstring2_gc(RING_API_STATE, pAttrPair, (const char *)value, (int)value_len);
			}
			else
			{
				ring_list_addstring_gc(RING_API_STATE, pAttrPair, "");
			}

			attr = lxb_dom_element_next_attribute(attr);
		}
	}

	RING_API_RETLIST(pResultList);
}

RING_FUNC(ring_html_node_set_inner_text)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISCPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_node_t *node = (lxb_dom_node_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	const lxb_char_t *text = (const lxb_char_t *)RING_API_GETSTRING(2);
	size_t text_len = (size_t)RING_API_GETSTRINGSIZE(2);

	if (node == NULL)
	{
		RING_API_ERROR("Invalid node pointer.");
		return;
	}

	lxb_status_t status = lxb_dom_node_text_content_set(node, text, text_len);
	if (status != LXB_STATUS_OK)
	{
		RING_API_ERROR("Failed to set inner text.");
		return;
	}

	RING_API_RETNUMBER(1);
}

RING_FUNC(ring_html_node_set_inner_html)
{
	if (RING_API_PARACOUNT != 2 || !RING_API_ISCPOINTER(1) || !RING_API_ISSTRING(2))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_element_t *element = (lxb_dom_element_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	const lxb_char_t *html = (const lxb_char_t *)RING_API_GETSTRING(2);
	size_t html_len = (size_t)RING_API_GETSTRINGSIZE(2);

	if (element == NULL)
	{
		RING_API_ERROR("Invalid element pointer.");
		return;
	}

	lxb_html_element_t *result = lxb_html_element_inner_html_set((lxb_html_element_t *)element, html, html_len);
	if (result == NULL)
	{
		RING_API_ERROR("Failed to set inner HTML.");
		return;
	}

	RING_API_RETNUMBER(1);
}

RING_FUNC(ring_html_get_root)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_html_document_t *document = (lxb_html_document_t *)RING_API_GETCPOINTER(1, RING_HTML_DOC);
	if (document != NULL)
	{
		lxb_dom_element_t *root = lxb_dom_document_element(&document->dom_document);
		if (root != NULL)
		{
			RING_API_RETMANAGEDCPOINTER(root, RING_HTML_NODE, NULL);
		}
	}
}

RING_FUNC(ring_html_get_head)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_html_document_t *document = (lxb_html_document_t *)RING_API_GETCPOINTER(1, RING_HTML_DOC);
	if (document != NULL)
	{
		lxb_html_head_element_t *head = lxb_html_document_head_element(document);
		if (head != NULL)
		{
			RING_API_RETMANAGEDCPOINTER(head, RING_HTML_NODE, NULL);
		}
	}
}

RING_FUNC(ring_html_node_first_child)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_node_t *node = (lxb_dom_node_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	if (node != NULL)
	{
		lxb_dom_node_t *child = node->first_child;

		while (child != NULL && child->type != LXB_DOM_NODE_TYPE_ELEMENT)
		{
			child = child->next;
		}
		if (child != NULL)
		{
			RING_API_RETMANAGEDCPOINTER(child, RING_HTML_NODE, NULL);
		}
	}
}

RING_FUNC(ring_html_node_last_child)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_node_t *node = (lxb_dom_node_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	if (node != NULL)
	{
		lxb_dom_node_t *child = node->last_child;

		while (child != NULL && child->type != LXB_DOM_NODE_TYPE_ELEMENT)
		{
			child = child->prev;
		}
		if (child != NULL)
		{
			RING_API_RETMANAGEDCPOINTER(child, RING_HTML_NODE, NULL);
		}
	}
}

RING_FUNC(ring_html_node_inner_html)
{
	if (RING_API_PARACOUNT != 1 || !RING_API_ISCPOINTER(1))
	{
		RING_API_ERROR(RING_API_BADPARATYPE);
		return;
	}
	lxb_dom_node_t *node = (lxb_dom_node_t *)RING_API_GETCPOINTER(1, RING_HTML_NODE);
	if (node != NULL)
	{
		lexbor_str_t str = {0};
		lxb_status_t status;

		lxb_dom_node_t *child = node->first_child;
		while (child != NULL)
		{
			status = lxb_html_serialize_tree_str(child, &str);
			if (status != LXB_STATUS_OK)
			{
				lexbor_str_destroy(&str, node->owner_document->text, false);
				RING_API_ERROR("Failed to serialize inner HTML.");
				return;
			}
			child = child->next;
		}

		if (str.length > 0)
		{
			RING_API_RETSTRING2((const char *)lexbor_str_data(&str), (int)lexbor_str_length(&str));
			lexbor_str_destroy(&str, node->owner_document->text, false);
		}
		else
		{
			RING_API_RETSTRING("");
		}
	}
}

RING_LIBINIT
{
	RING_API_REGISTER("html_parse", ring_html_parse);
	RING_API_REGISTER("html_get_body", ring_html_get_body);
	RING_API_REGISTER("html_find", ring_html_find);
	RING_API_REGISTER("html_find_attr", ring_html_find_attr);
	RING_API_REGISTER("html_node_text", ring_html_node_text);
	RING_API_REGISTER("html_node_html", ring_html_node_html);
	RING_API_REGISTER("html_node_tag", ring_html_node_tag);
	RING_API_REGISTER("html_node_attr", ring_html_node_attr);

	RING_API_REGISTER("html_node_parent", ring_html_node_parent);
	RING_API_REGISTER("html_node_next_sibling", ring_html_node_next_sibling);
	RING_API_REGISTER("html_node_prev_sibling", ring_html_node_prev_sibling);
	RING_API_REGISTER("html_node_children", ring_html_node_children);
	RING_API_REGISTER("html_node_has_attr", ring_html_node_has_attr);

	RING_API_REGISTER("html_create_element", ring_html_create_element);
	RING_API_REGISTER("html_create_text_node", ring_html_create_text_node);
	RING_API_REGISTER("html_node_append_child", ring_html_node_append_child);
	RING_API_REGISTER("html_node_insert_before", ring_html_node_insert_before);
	RING_API_REGISTER("html_node_insert_after", ring_html_node_insert_after);
	RING_API_REGISTER("html_node_remove", ring_html_node_remove);

	RING_API_REGISTER("html_node_set_attr", ring_html_node_set_attr);
	RING_API_REGISTER("html_node_remove_attr", ring_html_node_remove_attr);
	RING_API_REGISTER("html_node_get_all_attrs", ring_html_node_get_all_attrs);

	RING_API_REGISTER("html_node_set_inner_text", ring_html_node_set_inner_text);
	RING_API_REGISTER("html_node_set_inner_html", ring_html_node_set_inner_html);

	RING_API_REGISTER("html_get_root", ring_html_get_root);
	RING_API_REGISTER("html_get_head", ring_html_get_head);

	RING_API_REGISTER("html_node_first_child", ring_html_node_first_child);
	RING_API_REGISTER("html_node_last_child", ring_html_node_last_child);

	RING_API_REGISTER("html_node_inner_html", ring_html_node_inner_html);
}